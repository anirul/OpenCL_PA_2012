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

float2 square_c(float2 c) {
	return (float2)(c.x * c.x - c.y * c.y, 2.0f * c.y * c.x);
}

float2 mult_c(float2 c1, float2 c2) {
	return (float2)(c1.x * c2.x - c1.y * c2.y, c1.y * c2.x + c1.x * c2.y);
}

float2 add_c(float2 c1, float2 c2) {
	return (float2)(c1.x + c2.x, c1.y + c2.y);
}

float square_norm_c(float2 c) {
	return c.x * c.x + c.y * c.y;
}

__kernel void julia_buffer(
	__global float* out,
	float2 c, 
	uint max_iteration)
{
	const uint2 d = (uint2)(get_global_id(0), get_global_id(1));
	const uint2 m = (uint2)(get_global_size(0), get_global_size(1));
	float2 pos = (float2)(
		(((float)d.x / (float)m.x) * 4.0f) - 2.0f, 
		(((float)d.y / (float)m.y) * 4.0f) - 2.0f);
	uint ite = 0;
	while ((square_norm_c(pos) < 4.0f) && (ite < max_iteration)) {
		pos = add_c(square_c(pos), c);
		ite++;
	}
	float val = (float)ite / (float)max_iteration;
	out[d.y * m.x + d.x] = val;
}

__kernel void julia_image(
	__write_only image2d_t out,
	float2 c, 
	uint max_iteration)
{
	const sampler_t format = 
		CLK_NORMALIZED_COORDS_FALSE | 
		CLK_FILTER_NEAREST | 
		CLK_ADDRESS_CLAMP;
	const int2 d = (int2)(get_global_id(0), get_global_id(1));
	const int2 m = (int2)(get_global_size(0), get_global_size(1));
	float2 pos = (float2)(
		(((float)d.x / (float)m.x) * 4.0f) - 2.0f, 
		(((float)d.y / (float)m.y) * 4.0f) - 2.0f);
	uint ite = 0;
	while ((square_norm_c(pos) < 4.0f) && (ite < max_iteration)) {
		pos = add_c(square_c(pos), c);
		ite++;
	}
	float val = (float)ite / (float)max_iteration;
	write_imagef(out, d, (float4)(val, val, val, val));
}

