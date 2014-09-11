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

#ifndef CL_cellular_automata_HEADER_DEFINED
#define CL_cellular_automata_HEADER_DEFINED

class cl_cellular_automata {
private :
	cl::Buffer cl_buffer_out_;
	cl::Buffer cl_buffer_in_;
	cl::Image2D cl_image_out_;
	cl::Image2D cl_image_in_;
	cl::size_t<3> region_;
	cl::size_t<3> origin_;
	size_t buffer_size_;
	size_t total_size_;
	cl_uint mdx_;
	cl_uint mdy_;
	cl_uint max_iterations_;
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
	cl_cellular_automata(bool gpu, unsigned int device = 0);
	void init(const std::string& cl_file);
	// if change size
	void setup(
		const std::pair<unsigned int, unsigned int>& s,
		unsigned int max_iterations = 1);
	// then before the next iteration
	void prepare_image(const std::vector<char>& in);
	void prepare_buffer(const std::vector<float>& in);
	// run the actual next step
	boost::posix_time::time_duration run_image(std::vector<char>& out);
	boost::posix_time::time_duration run_buffer(std::vector<float>& out);
};

#endif // CL_cellular_automata_HEADER_DEFINED
