/*
 * Copyright (c) 2012, Frederic Dubouchet
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the CERN nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Frederic Dubouchet ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Frederic DUBOUCHEDT BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <iostream>
#include <vector>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#ifdef __linux__
#include <GL/glx.h>
#endif
#include <boost/date_time/posix_time/posix_time.hpp>

#include "cl_julia.h"

using namespace boost::posix_time;

cl_julia::cl_julia(bool gpu) {
	//setup devices and context
	std::vector<cl::Platform> platforms;
	err_ = cl::Platform::get(&platforms);
	device_used_ = 0;
	err_ = platforms[0].getDevices((gpu) ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, &devices_);
	int t = devices_.front().getInfo<CL_DEVICE_TYPE>();
	try {
		throw cl::Error(0, "cheat pass");
		#if defined (__APPLE__) || defined(MACOSX)
			CGLContextObj kCGLContext = CGLGetCurrentContext();
			CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
			cl_context_properties props[] =
			{
				CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
				0
			};
			context_ = cl::Context(props);
		#else
			#if defined WIN32 // Win32
				cl_context_properties props[] =
				{
					CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
					CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
					CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
					0
				};
				context_ = cl::Context(CL_DEVICE_TYPE_GPU, props);
			#else
				throw cl::Error(0, "cheat pass");
				cl_context_properties props[] =
				{
					CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
					CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
					CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
					0
				};
				context_ = cl::Context(CL_DEVICE_TYPE_GPU, props);
			#endif
		#endif
	} catch (cl::Error er) {
		std::cerr << "Warning         : could not attach GL and CL toghether!" << std::endl;
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
		context_ = cl::Context((gpu) ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU , properties);
		devices_ = context_.getInfo<CL_CONTEXT_DEVICES>();
	}
	queue_ = cl::CommandQueue(context_, devices_[device_used_], 0, &err_);
}

void cl_julia::init() {
	std::string file_name = "./julia.cl";
	FILE* file = fopen(file_name.c_str(), "rt");
	if (!file) throw std::runtime_error("could not open file " + file_name);
	const unsigned int BUFFER_SIZE = 4096;
	size_t bytes_read = 0;
	std::string kernel_source = "";
	do {
		char temp[BUFFER_SIZE];
		memset(temp, 0, BUFFER_SIZE);
		bytes_read = fread(temp, sizeof(char), BUFFER_SIZE, file);
		kernel_source += temp;
	} while (bytes_read != 0);
	fclose(file);
	cl::Program::Sources source(
		1, 
		std::make_pair(kernel_source.c_str(), 
		kernel_source.size()));
	program_ = cl::Program(context_, source);
	try {
		err_ = program_.build(devices_);
	} catch (cl::Error er) {
		std::cout << "build status    : " 
			<< program_.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices_[0]) << std::endl;
		std::cout << "build options   : " 
			<< program_.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices_[0]) << std::endl;
		std::cout << "build log       : " 
			<< program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices_[0]) << std::endl;
		throw er;
	}
}

void cl_julia::setup(
	const std::pair<float, float>& c,
	const std::pair<unsigned int, unsigned int>& s,
	unsigned int max_iterations) 
{
	cl_c_.x = c.first;
	cl_c_.y = c.second;
	max_iterations_ = max_iterations;
	mdx_ = s.first;
	mdy_ = s.second;
}

void cl_julia::prepare_buffer() {
	total_size_ = mdx_ * mdy_;
	kernel_ = cl::Kernel(program_, "julia_buffer", &err_);
	//initialize our CPU memory arrays, send them to the device and set the kernel arguements
	cl_buffer_out_ = cl::Buffer(
		context_, 
		CL_MEM_WRITE_ONLY, 
		sizeof(float) * total_size_, 
		NULL, 
		&err_);
	queue_.finish();
	//set the arguements of our kernel
	err_ = kernel_.setArg(0, cl_buffer_out_);
	err_ = kernel_.setArg(1, cl_c_);
	err_ = kernel_.setArg(2, max_iterations_);
	//Wait for the command queue to finish these commands before proceeding
	queue_.finish();
}

void cl_julia::prepare_image() {
	total_size_ = mdx_ * mdy_;
	origin_.push_back(0);
	origin_.push_back(0);
	origin_.push_back(0);
	region_.push_back(mdx_);
	region_.push_back(mdy_);
	region_.push_back(1);
	kernel_ = cl::Kernel(program_, "julia_image", &err_);
	//initialize our CPU memory arrays, send them to the device and set the kernel_ arguements
	cl::ImageFormat format = cl::ImageFormat(CL_INTENSITY, CL_UNORM_INT8);
	cl_image_out_ = cl::Image2D(
		context_,
		CL_MEM_WRITE_ONLY,
		format,
		mdx_,
		mdy_,
		0,
		NULL,
		&err_);
	queue_.finish();
	//set the arguements of our kernel_
	err_ = kernel_.setArg(0, cl_image_out_);
	err_ = kernel_.setArg(1, cl_c_);
	err_ = kernel_.setArg(2, max_iterations_);
	//Wait for the command queue to finish these commands before proceeding
	queue_.finish();	
}

time_duration cl_julia::run_buffer(std::vector<float>& out) {
	ptime before;
	ptime after;
	if (out.size() != total_size_)
		out.resize(total_size_);
	before = microsec_clock::universal_time();
	err_ = queue_.enqueueNDRangeKernel(
		kernel_, 
		cl::NullRange, 
		cl::NDRange(mdx_, mdy_), 
		cl::NullRange, 
		NULL, 
		&event_); 
	queue_.finish();
	after = microsec_clock::universal_time();
	err_ = queue_.enqueueReadBuffer(
		cl_buffer_out_, 
		CL_TRUE, 
		0, 
		sizeof(float) * total_size_, 
		&out[0], 
		NULL, 
		&event_);
	queue_.finish();
	return (after - before);
}

time_duration cl_julia::run_image(std::vector<char>& out) {
	ptime before;
	ptime after;
	if (out.size() != total_size_)
		out.resize(total_size_);
	before = microsec_clock::universal_time();
	err_ = queue_.enqueueNDRangeKernel(
		kernel_, 
		cl::NullRange, 
		cl::NDRange(mdx_, mdy_), 
		cl::NullRange, 
		NULL, 
		&event_); 
	queue_.finish();
	after = microsec_clock::universal_time();
	err_ = queue_.enqueueReadImage(
		cl_image_out_, 
		CL_TRUE, 
		origin_, 
		region_, 
		mdx_ * sizeof(char), 
		0, 
		(void*)&out[0]);
	queue_.finish();
	return (after - before);
}

