/*
 * Copyright (c) 2012, Frederic DUBOUCHET
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
 * THIS SOFTWARE IS PROVIDED BY Frederic DUBOUCHET ``AS IS'' AND ANY
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

#include <iostream>
#include <vector>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "cl_util.h"
#include "glut_win.h"
#include "cl_cellular_automata.h"
#include "win_cellular_automata.h"

using namespace cl;
using namespace boost::program_options;
using namespace boost::posix_time;

int main(int ac, char** av) {
	bool no_window = false;
	bool enable_gpu = true;
	bool enable_image = false;
	unsigned int max_height = 512;
	unsigned int max_width = 512;
	unsigned int max_iterations = 10;
	unsigned int nb_loops = 100;
	std::string cl_file = "life.cl";
	try {
		// parse command line
		options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("opencl-cpu,c", "compute using openCL on CPU")
			("opencl-gpu,g", "compute using openCL on GPU (default)")
			("image,m", "use OpenCL Image2D instead of buffer") 
			("width,w", value<unsigned int>(), "image width")
			("height,t", value<unsigned int>(), "image height")
			("iterations,i", value<unsigned int>(), "julia iterations")
			("cl-file,f", value<std::string>(), 
				"OpenCL file to be compiled (default : life.cl)")
			("loop,l", value<unsigned int>(), 
				"number of loops (only in no-window mode)")
			("no-window,n", "no window output")
		;
		variables_map vm;
		store(command_line_parser(ac, av).options(desc).run(), vm);
		if (vm.count("help")) {
			std::cout << desc << std::endl;
			return 1;
		}
		if (vm.count("opencl-cpu")) {
			enable_gpu = false;
			std::cout << "OpenCL (CPU)    : enable" << std::endl;
		} 
		if (vm.count("opencl-gpu")) {
			enable_gpu = true;
			std::cout << "OpenCL (GPU)    : enable" << std::endl;
		} 
		if (vm.count("cl-file")) {
			cl_file = vm["cl-file"].as<std::string>();
			std::cout << "OpenCL file     : " << cl_file << std::endl;
		}
		if (vm.count("width")) {
			max_width = vm["width"].as<unsigned int>();
		}
		std::cout << "Width           : " << max_width << std::endl; 
		if (vm.count("height")) {
			max_height = vm["height"].as<unsigned int>();
		}
		std::cout << "Height          : " << max_height << std::endl;
		if (vm.count("iterations")) {
			max_iterations = vm["iterations"].as<unsigned int>();
		}
		std::cout << "Iterations      : " << max_iterations << std::endl;
		if (vm.count("no-window")) {
			no_window = true;
			std::cout << "Window          : no" << std::endl;
		} else {
			std::cout << "Window          : yes" << std::endl;
		}
		if (vm.count("loop")) {
			nb_loops = vm["loop"].as<unsigned int>();
		}
		if (vm.count("image")) {
			enable_image = true;
			std::cout << "OpenCL Image2D  : enable" << std::endl;
		} else {
			enable_image = false;
			std::cout << "OpenCL Image2D  : disable" << std::endl;
		}
		// create the initial state (random)
		std::vector<float> initial_buffer;
		std::vector<char> initial_image;
		if (enable_image) {
			initial_image.resize(max_width * max_height);
			for (int i = 0; i < max_height * max_width; ++i) {
				initial_image[i] = (char)(((random() % 2) == 0) ? 0xff : 0x0000);
			}
		} else {
			initial_buffer.resize(max_width * max_height);
			for (int i = 0; i < max_height * max_width; ++i)
				initial_buffer[i] = (float)(((random() % 2) == 0) ? 1.0f : 0.0f);
		}
		// do the actual job
		if (!no_window) {
			win_cellular_automata* pwca = NULL;
			if (enable_image)
				pwca = new win_cellular_automata(
					std::make_pair<unsigned int, unsigned int>(max_width, max_height),
					initial_image,
					cl_file,
					enable_gpu,
					max_iterations);
			else
				pwca = new win_cellular_automata(
					std::make_pair<unsigned int, unsigned int>(max_width, max_height),
					initial_buffer,
					cl_file,
					enable_gpu,
					max_iterations);
			glut_win* pwin = glut_win::instance(
				std::string("Cellular Automata"),
				std::make_pair<unsigned int, unsigned int>(max_width, max_height),
				pwca);
			pwin->run();
			delete pwca;
		} else {
			cl_cellular_automata cca(enable_gpu);
			cca.init("life.cl");
			cca.setup(
				std::make_pair<unsigned int, unsigned int>(max_width, max_height),
				max_iterations);
			time_duration best_time = minutes(60);
			for (int i = 0; i < nb_loops; ++i) {
				time_duration actual_time;
				if (enable_image) {
					cca.prepare_image(initial_image);
					actual_time = cca.run_image(initial_image);
				} else {
					cca.prepare_buffer(initial_buffer);
					actual_time = cca.run_buffer(initial_buffer);
				}
				if (actual_time < best_time) best_time = actual_time;
				std::cout 
					<< "\riteration [" << i + 1 << "/" << nb_loops << "] : "
					<< "actual_time : " << actual_time;
				std::cout.flush();
			}
			std::cout << std::endl 
				<< "Finished : [" << nb_loops << "] Best time : " << best_time 
				<< std::endl;
		}
	// error handling
	} catch (cl::Error er) {
		std::cerr 
			<< "exception (CL)  : " << er.what() 
			<< "(" << er << ")" << std::endl;
		return -2;		
	} catch (std::exception& ex) {
		std::cerr << "exception (std) : " << ex.what() << std::endl;
		return -1;
	}
	return 0;
}

