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

#include <stdexcept>
#include <string>
#include <vector>
#ifdef __linux__
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#ifdef __APPLE__
#include <glut/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "cl_cellular_automata.h"
#include "glut_win.h"
#include "win_cellular_automata.h"

using namespace boost::posix_time;

win_cellular_automata::win_cellular_automata(
	const std::pair<unsigned int, unsigned int>& range,
	const std::vector<float>& initial_buffer,
	const std::string& cl_file,
	bool gpu,
	unsigned int device,
	unsigned int max_iterations)
	:	range_(range),
		max_iterations_(max_iterations),
		gpu_(gpu),
		device_(device),
		image_(false),
		p_cellular_automata_(NULL),
		cl_file_(cl_file)
{
	current_buffer_ = initial_buffer;
	best_time_ = minutes(60);
}

win_cellular_automata::win_cellular_automata(
	const std::pair<unsigned int, unsigned int>& range,
	const std::vector<char>& initial_image,
	const std::string& cl_file,
	bool gpu,
	unsigned int device,
	unsigned int max_iterations)
	:	range_(range),
		max_iterations_(max_iterations),
		gpu_(gpu),
		device_(device),
		image_(true),
		p_cellular_automata_(NULL),
		cl_file_(cl_file)
{
	current_image_ = initial_image;
	best_time_ = minutes(60);
}


win_cellular_automata::~win_cellular_automata() {}

void win_cellular_automata::init() {
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(-1, 1, -1, 1);
	glGenTextures(1, &texture_id_);
	p_cellular_automata_ = new cl_cellular_automata(gpu_, device_);
	p_cellular_automata_->init(cl_file_);
	p_cellular_automata_->setup(range_, max_iterations_);
}

void win_cellular_automata::display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2f(0, 1);
			glVertex2f(-1, 1);
			glTexCoord2f(1, 1);
			glVertex2f(1, 1);
			glTexCoord2f(1, 0);
			glVertex2f(1, -1);
			glTexCoord2f(0, 0);
			glVertex2f(-1, -1);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glFlush();
	glutPostRedisplay();
}

void win_cellular_automata::idle() {
	glFinish();
	if (p_cellular_automata_) {
		time_duration actual_time;
		if (image_) {
			p_cellular_automata_->prepare_image(current_image_);
			actual_time = p_cellular_automata_->run_image(current_image_);
		} else {
			p_cellular_automata_->prepare_buffer(current_buffer_);
			actual_time = p_cellular_automata_->run_buffer(current_buffer_);
		}
		if (actual_time < best_time_) best_time_ = actual_time;
		std::cout << "\rCompute time    : " << actual_time << " best " << best_time_;
		std::cout.flush();
	}
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (image_) {
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			1,
			range_.first,
			range_.second,
			0,
			GL_LUMINANCE,
			GL_UNSIGNED_BYTE,
			&current_image_[0]);
	} else {
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			1,
			range_.first,
			range_.second,
			0,
			GL_LUMINANCE,
			GL_FLOAT,
			&current_buffer_[0]);
	}
	glFinish();
}

void win_cellular_automata::reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glFinish();
}

void win_cellular_automata::mouse_event(int button, int state, int x, int y) {}

void win_cellular_automata::mouse_move(int x, int y) {}

void win_cellular_automata::keyboard(unsigned char key, int x, int y) {}

void win_cellular_automata::finish() {
	if (p_cellular_automata_) {
		delete p_cellular_automata_;
		p_cellular_automata_ = NULL;
	}
	std::cout << std::endl;
}
