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
 
#ifndef WIN_JULIA_HEADER_DEFINED
#define WIN_JULIA_HEADER_DEFINED

class win_julia : public i_win {
private :
	std::pair<float, float> c_;
	std::pair<unsigned int, unsigned int> range_;
	unsigned int max_iterations_;
	std::vector<float> current_buffer_;
	std::vector<char> current_image_;
	cl_julia* p_julia_;
	bool gpu_;
	bool image_;
	unsigned int texture_id_;
	boost::posix_time::time_duration best_time_;
public :
	// constructor size of image and image pointer
	win_julia(
		const std::pair<float, float>& c,
		const std::pair<unsigned int, unsigned int>& range,
		bool gpu,
		bool image,
		unsigned int max_iterations);
	virtual ~win_julia();
public :
	// inherited from the i_win interface
	virtual void init();
	virtual void display();
	virtual void idle();
	virtual void reshape(int w, int h);
	virtual void mouse_event(int button, int state, int x, int y);
	virtual void mouse_move(int x, int y);
	virtual void keyboard(unsigned char key, int x, int y);
	virtual void finish();
};

#endif // WIN_JULIA_HEADER_DEFINED
