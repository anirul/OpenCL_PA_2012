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
 * DISCLAIMED. IN NO EVENT SHALL Frederic DUBOUCHET BE LIABLE FOR ANY
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

#include "cl_fft.h"

using namespace boost::posix_time;

cl_fft::cl_fft(bool gpu) {
   //setup devices_ and context_
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

void cl_fft::init(const std::string& cl_file) {
   FILE* file = fopen(cl_file.c_str(), "rt");
   if (!file) throw std::runtime_error("could not open file " + cl_file);
   const unsigned int BUFFER_SIZE = 4096;
   size_t bytes_read = 0;
   std::string kernel__source = "";
   do {
      char temp[BUFFER_SIZE];
      memset(temp, 0, BUFFER_SIZE);
      bytes_read = fread(temp, sizeof(char), BUFFER_SIZE, file);
      kernel__source += temp;
   } while (bytes_read != 0);
   fclose(file);
   cl::Program::Sources source(
         1, 
         std::make_pair(kernel__source.c_str(), 
            kernel__source.size()));
   program_ = cl::Program(context_, source);
   try {
      err_ = program_.build(devices_);
   } catch (cl::Error er) {
      std::cerr << "build status    : " 
         << program_.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices_[0]) << std::endl;
      std::cerr << "build options   : " 
         << program_.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices_[0]) << std::endl;
      std::cerr << "build log       : "  << std::endl
         << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices_[0]) << std::endl;
      throw er;
   }
}

void cl_fft::prepare(const std::vector<cl_float2>& in) {
   data_size_ = in.size();
   kernel_ = cl::Kernel(program_, "fftRadix2Kernel", &err_);
   std::vector<cl_float2>::const_iterator ite;
   //initialize our CPU memory arrays, send them to the device and set the kernel_ arguements
   cl_buffer_in_x_ = cl::Buffer(
         context_, 
         CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
         sizeof(cl_float2) * data_size_, 
         (void*)&in[0], 
         &err_);
   cl_buffer_out_y_ = cl::Buffer(
         context_,
         CL_MEM_READ_WRITE,
         sizeof(cl_float2) * data_size_,
         NULL,
         &err_);
   queue_.finish();
   //set the arguements of our kernel_
   err_ = kernel_.setArg(0, cl_buffer_in_x_);
   err_ = kernel_.setArg(1, cl_buffer_out_y_);
   //Wait for the command queue_ to finish these commands before proceeding
   queue_.finish();	
}

time_duration cl_fft::run(std::vector<cl_float2>& out) {
   ptime before;
   ptime after;
   time_duration total = seconds(0);
   if (out.size() != data_size_)
      out.resize(data_size_);
   double p = log2(data_size_);
   for (int i = 1; i <= (data_size_ >> 1); i *= 2) {
      before = microsec_clock::universal_time();
      // enqueue the new parameter p
      err_ = kernel_.setArg(2, i);
      // make the computation
      err_ = queue_.enqueueNDRangeKernel(
            kernel_, 
            cl::NullRange, 
            cl::NDRange(data_size_ >> 1), 
            cl::NullRange, 
            NULL, 
            &event_); 
      queue_.finish();
      after = microsec_clock::universal_time();
      if (i != (data_size_ >> 1)) {
         err_ = queue_.enqueueCopyBuffer(
               cl_buffer_out_y_,
               cl_buffer_in_x_,
               0,
               0,
               data_size_ * sizeof(cl_float2),
               NULL,
               &event_);
         queue_.finish();
      }
      total += (after - before);
   }
   err_ = queue_.enqueueReadBuffer(
         cl_buffer_out_y_, 
         CL_TRUE, 
         0, 
         data_size_ * sizeof(cl_float2), 
         &out[0], 
         NULL, 
         &event_);
   queue_.finish();
   return total;
}

