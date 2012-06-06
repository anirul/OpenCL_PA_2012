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

int pos2linear(
	const int2 position,
	const int2 m) 
{
	return position.y * m.x + position.x;
}

int add(__global const float* in, const int2 position, const int2 m) {
	// empty border
	return 
		((position.x < 0 || position.x >= m.x) || (position.y < 0 || position.y >= m.y)) 
			? 0.0f 
			: (in[pos2linear(position, m)]) ? 1.0f : 0.0f;
}

__kernel void life_buffer(
	__global float* in,
	__global float* out,
	unsigned int max_iterations)
{
	const int2 d = (int2)(get_global_id(0), get_global_id(1));
	const int2 m = (int2)(get_global_size(0), get_global_size(1));
	
	int position = pos2linear(d, m);
	int sum = 0;
	sum += add(in, (int2)(d.x - 1, d.y + 1), m);
	sum += add(in, (int2)(d.x    , d.y + 1), m);
	sum += add(in, (int2)(d.x + 1, d.y + 1), m);
	sum += add(in, (int2)(d.x - 1, d.y    ), m);
	sum += add(in, (int2)(d.x + 1, d.y    ), m);
	sum += add(in, (int2)(d.x - 1, d.y - 1), m);
	sum += add(in, (int2)(d.x    , d.y - 1), m);
	sum += add(in, (int2)(d.x + 1, d.y - 1), m);	
	// wait for the other task to finish calculating
	barrier(CLK_GLOBAL_MEM_FENCE);
	out[position] = ((sum == 3) || (in[position] && (sum == 2))) ? 1.0f : 0.0f;
}

__kernel void life_image(
	__read_only image2d_t in,
	__write_only image2d_t out,
	unsigned int max_iterations)
{
	const sampler_t format = 
		CLK_NORMALIZED_COORDS_FALSE | 
		CLK_FILTER_NEAREST | 
		CLK_ADDRESS_CLAMP;
	const int2 d = (int2)(get_global_id(0), get_global_id(1));
	const float4 white = (float4)(1.0f, 1.0f, 1.0f, 1.0f);
	const float4 black = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	float4 center_col = read_imagef(in, format, d);
	float4 acc = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	acc += read_imagef(in, format, (int2)(d.x - 1, d.y + 1));
	acc += read_imagef(in, format, (int2)(d.x    , d.y + 1));
	acc += read_imagef(in, format, (int2)(d.x + 1, d.y + 1));
	acc += read_imagef(in, format, (int2)(d.x - 1, d.y    ));
	acc += read_imagef(in, format, (int2)(d.x + 1, d.y    ));
	acc += read_imagef(in, format, (int2)(d.x - 1, d.y - 1));
	acc += read_imagef(in, format, (int2)(d.x    , d.y - 1));
	acc += read_imagef(in, format, (int2)(d.x + 1, d.y - 1));
	// wait for the other task to finish calculating
	barrier(CLK_GLOBAL_MEM_FENCE);
	// compute futur state
	float4 col = ((acc.x == 3.0f) || ((center_col.x != 0.0f) && (acc.x == 2.0f))) 
		? white 
		: black;
	write_imagef(out, d, col);
}
