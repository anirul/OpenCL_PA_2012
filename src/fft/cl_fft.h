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

#ifndef CL_fft_HEADER_DEFINED
#define CL_fft_HEADER_DEFINED

class cl_fft {
private :
	cl::Buffer cl_buffer_in_x_;
   cl::Buffer cl_buffer_out_y_;
	size_t data_size_;
	unsigned int device_used_;
	std::vector<cl::Device> devices_;
	cl::Context context_;
	cl::CommandQueue queue_;
	cl::Program program_;
	cl::Kernel kernel_;
	// debugging variables	
	cl_int err_;
	cl::Event event_;
public :
	cl_fft(bool gpu);
	void init(const std::string& cl_file);
	// then before the next iteration
	void prepare(const std::vector<cl_float2>& in);
//   void prepare(const std::vector<double>& in);
	// run the actual next step
	boost::posix_time::time_duration run(std::vector<cl_float2>& out);
//   boost::posix_time::time_duration run(std::vector<double>& out);
};

#endif // CL_fft_HEADER_DEFINED

