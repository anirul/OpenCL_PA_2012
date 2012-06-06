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

#include <iostream>
#include <vector>
#include <complex>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "cl_util.h"
#include "cl_fft.h"
#ifdef WITH_FFTW
#include "fftw_fft.h"
#endif

#include "txt_file.h"

using namespace cl;
using namespace boost::program_options;
using namespace boost::posix_time;

int main(int ac, char** av) {
   bool enable_gpu = true;
   bool enable_fftw = false;
   unsigned int nb_loops = 1;
   std::string cl_file = "fft.cl";
   std::string graph_in = "input.csv";
   std::string graph_out = "";
   try {
      // parse command line
      options_description desc("Allowed options");
      desc.add_options()
         ("help,h", "produce help message")
         ("opencl-cpu,c", "compute using openCL on CPU")
         ("opencl-gpu,g", "compute using openCL on GPU (default)")
#ifdef WITH_FFTW
         ("fftw,w", "compute using the FFTW3 library")
#endif // WITH_FFTW
         ("file-in,f", value<std::string>(), 
          "Input file in TXT format")
         ("file-out,o", value<std::string>(),
          "Output file in TXT format")
         ("loop,l", value<unsigned int>(), 
          "number of loops")
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
      if (vm.count("file-in")) {
         graph_in = vm["file-in"].as<std::string>();
      }
      std::cout << "Graph file (in) : " << graph_in << std::endl;
      if (vm.count("file-out")) {
         graph_out = vm["file-out"].as<std::string>();
         std::cout << "Graph file (out): " << graph_out << std::endl;
      }
      if (vm.count("loop")) {
         nb_loops = vm["loop"].as<unsigned int>();
      }
#ifdef WITH_FFTW
      if (vm.count("fftw")) {
         enable_fftw = true;
      } else {
         enable_fftw = false;
      }
#endif // WITH_FFTW
      std::cout << "Loop            : " << nb_loops << std::endl;
      if (!enable_fftw) { // OpenCL
         // read the file with the data
         txt_file<cl_float2> ef;
         std::vector<cl_float2> out;
         ef.import_file(graph_in);
         // do the actual job
         cl_fft cfw(enable_gpu);
         cfw.init("fft.cl");
         std::cout 
            << "Setup           : (" << ef.size() << ")" << std::endl;
         time_duration best_time = minutes(60);
         time_duration sum_time = seconds(0);
         for (int i = 0; i < nb_loops; ++i) {
            time_duration actual_time;
            cfw.prepare(ef);
            actual_time = cfw.run(out);
            if (actual_time < best_time) best_time = actual_time;
            sum_time += actual_time;
            std::cout 
               << "\riteration [" << i + 1 << "/" << nb_loops << "] : "
               << "actual_time : " << actual_time;
            std::cout.flush();
         }
         std::cout << std::endl 
            << "Finished        : [" << nb_loops << "]"
            << std::endl;
         std::cout << "Best time       : " << best_time << std::endl;
         std::cout << "Average time    : " << sum_time / nb_loops << std::endl;
         // export to file out (in case exist)
         if (graph_out.size()) {
            ef = out;
            ef.export_file(graph_out);
         }
         if (ef.size() <= 256) std::cout << ef;
      }
#ifdef WITH_FFTW
      else { // FFTW
         // read the fiel with the data
         txt_file<std::complex<float> > ef;
         std::vector<std::complex<float> > out;
         ef.import_file(graph_in);
         fftw_fft<float> cfw;
         std::cout 
            << "Setup           : (" << ef.size() << ")" << std::endl;
         time_duration best_time = minutes(60);
         time_duration sum_time = seconds(0);
         for (int i = 0; i < nb_loops; ++i) {
            time_duration actual_time;
            cfw.prepare(ef);
            actual_time = cfw.run(out);
            if (actual_time < best_time) best_time = actual_time;
            sum_time += actual_time;
            std::cout 
               << "\riteration [" << i + 1 << "/" << nb_loops << "] : "
               << "actual_time : " << actual_time;
            std::cout.flush();
         }
         std::cout << std::endl 
            << "Finished        : [" << nb_loops << "]"
            << std::endl;
         std::cout << "Best time       : " << best_time << std::endl;
         std::cout << "Average time    : " << sum_time / nb_loops << std::endl;
         // export to file out (in case exist)
         if (graph_out.size()) {
            ef = out;
            ef.export_file(graph_out);
         }
         if (ef.size() <= 256) std::cout << ef;
      }
#endif // WITH_FFTW
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

