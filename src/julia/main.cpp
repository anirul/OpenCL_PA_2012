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

#include <iostream>
#include <vector>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "cl_julia.h"
#include "cl_util.h"
#include "glut_win.h"
#include "win_julia.h"

using namespace cl;
using namespace boost::posix_time;
using namespace boost::program_options;

int main(int ac, char** av) {
	bool no_window = false;
	bool enable_gpu = true;
	bool enable_image = false;
	int device = 0;
	unsigned int max_height = 512;
	unsigned int max_width = 512;
	unsigned int max_iterations = 1000;
	unsigned int nb_loops = 1000;
	float cx = -0.1f;
	float cy = 0.651f;
	float radius = 0.0f;
	try {
		options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("opencl-cpu,c", "compute using openCL on CPU")
			("opencl-gpu,g", "compute using openCL on GPU")
			("device,d", value<unsigned int>(), "device to be selected")
			("image,m", "use OpenCL Image2D instead of buffer")
			("width,w", value<unsigned int>(), "image width")
			("height,t", value<unsigned int>(), "image height")
			("iterations,i", value<unsigned int>(), "julia iterations")
			("loop,l", value<unsigned int>(),
				"number of loops (only in no-window mode)")
			("cx,x", value<float>(), "value of c(x)")
			("cy,y", value<float>(), "value of c(y)")
			("radius,r", value<float>(), "radius around c")
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
		} else if (vm.count("opencl-gpu")) {
			enable_gpu = true;
			std::cout << "OpenCL (GPU)    : enable" << std::endl;
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
		if (vm.count("loop")) {
			nb_loops = vm["loop"].as<unsigned int>();
		}
		std::cout << "Loops           : " << nb_loops << std::endl;
			if (vm.count("cx")) {
			cx = vm["cx"].as<float>();
		}
		std::cout << "Cx              : " << cx << std::endl;
		if (vm.count("cy")) {
			cy = vm["cy"].as<float>();
		}
		std::cout << "Cy              : " << cy << std::endl;
		if (vm.count("radius")) {
			radius = vm["radius"].as<float>();
		}
		std::cout << "radius          : " << radius << std::endl;
		if (vm.count("no-window")) {
			no_window = true;
			std::cout << "Window          : no" << std::endl;
		} else {
			std::cout << "Window          : yes" << std::endl;
		}
		if (vm.count("image")) {
			enable_image = true;
			std::cout << "OpenCL Image2D  : enable" << std::endl;
		} else {
			enable_image = false;
			std::cout << "OpenCL Image2D  : disable" << std::endl;
		}
		if (vm.count("device")) {
			device = vm["device"].as<unsigned int>();
		}
	} catch (std::exception& e) {
		std::cerr << "exception     : " << e.what() << std::endl;
		return -1;
	}
	try {
		if (!no_window) {
			win_julia wjulia(
				std::make_pair<float, float>(cx, cy),
				radius,
				std::make_pair<unsigned int, unsigned int>(max_width, max_height),
				enable_gpu,
				device,
				enable_image,
				max_iterations);
			glut_win* pwin = glut_win::instance(
				std::string("Julia"),
				std::make_pair<unsigned int, unsigned int>(max_width, max_height),
				&wjulia);
			pwin->run();
		} else {
			std::vector<float> vec_buffer(max_width * max_height);
			std::vector<char> vec_image(max_width * max_height);
			cl_julia* pjulia = new cl_julia(enable_gpu, device);
			pjulia->init();
			pjulia->setup(
				std::make_pair<float, float>(cx, cy),
				radius,
				std::make_pair<unsigned int, unsigned int>(max_height, max_width),
				max_iterations);
			time_duration best_time = minutes(60);
			for (int i = 0; i < nb_loops; ++i) {
				time_duration actual_time;
				if (enable_image) {
					pjulia->prepare_image();
					actual_time = pjulia->run_image(vec_image);
				} else {
						pjulia->prepare_buffer();
					actual_time = pjulia->run_buffer(vec_buffer);
				}
				if (actual_time < best_time) best_time = actual_time;
				std::cout
					<< "\riteration [" << i + 1 << "/" << nb_loops << "] : "
					<< "actual_time : " << actual_time;
				std::cout.flush();
			}
			std::cout << std::endl << "Finished : [" << nb_loops << "] Best time : "
				<< best_time << std::endl;
			delete pjulia;
		}
	} catch (cl::Error er) {
		std::cerr << "exception (CL)  : " << er.what() << "(" << er << ")" << std::endl;
		return -1;
	} catch (std::exception& ex) {
		std::cerr << "exception (STL) : " << ex.what() << std::endl;
		return -2;
	}
	return 0;
}
