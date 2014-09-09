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

#include "cl_julia.h"
#include "glut_win.h"
#include "win_julia.h"

using namespace boost::posix_time;

win_julia::win_julia(
	const std::pair<float, float>& c,
	const float r,
	const std::pair<unsigned int, unsigned int>& range,
	bool gpu,
	bool image,
	unsigned int max_iterations)
	: 	c_(c),
		r_(r),
		range_(range),
		max_iterations_(max_iterations),
		p_julia_(NULL),
		gpu_(gpu),
		image_(image)
{
	best_time_ = minutes(60);
}

win_julia::~win_julia() {}

void win_julia::init() {
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(-1, 1, -1, 1);
	glGenTextures(1, &texture_id_);
	p_julia_ = new cl_julia(gpu_);
	p_julia_->init();
	p_julia_->setup(c_, r_, range_, max_iterations_);
} 

void win_julia::display() {
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

void win_julia::idle() {
	glFinish();
	if (p_julia_) {
		time_duration actual_time;
		if (image_) {
			p_julia_->prepare_image();
			actual_time = p_julia_->run_image(current_image_);
		} else {
			p_julia_->prepare_buffer();
			actual_time = p_julia_->run_buffer(current_buffer_);
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

void win_julia::reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glFinish();
} 

void win_julia::mouse_event(int button, int state, int x, int y) {} 

void win_julia::mouse_move(int x, int y) {} 

void win_julia::keyboard(unsigned char key, int x, int y) {} 

void win_julia::finish() {
	if (p_julia_) {
		delete p_julia_;
		p_julia_ = NULL;
	}
	std::cout << std::endl;
} 

