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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __APPLE__
	#include <OpenCL/cl.h>
#else
	#include <CL/cl.h>
#endif // __APPLE__
#include <string>
#include <sstream>
#include <iostream>

std::string GetDeviceInfoString(
	cl_device_id device_id,
	cl_device_info device_info) 
{
	char string_temp[512];
	size_t ret_size = 0;
	memset(string_temp, 0, 512);
	cl_int err = clGetDeviceInfo(
		device_id,
		device_info,
		512,
		string_temp,
		&ret_size);
	return std::string(string_temp);
}

std::string GetPlatformInfoString(
	cl_platform_id platform_id,
	cl_platform_info platform_info)
{
	char string_temp[512];
	size_t ret_size = 0;
	memset(string_temp, 0, 512);
	cl_int err = clGetPlatformInfo(
		platform_id,
		platform_info,
		512,
		string_temp,
		&ret_size);
	return std::string(string_temp);
}

bool GetDeviceInfoBool(
	cl_device_id device_id,
	cl_device_info device_info)
{
	cl_bool ret_bool;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		device_info,
		sizeof(cl_bool),
		&ret_bool,
		&ret_size);
	return ((ret_bool) ? true : false);
}

unsigned int GetDeviceInfoUInt(
	cl_device_id device_id,
	cl_device_info device_info)
{
	cl_uint ret_uint;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		device_info,
		sizeof(cl_uint),
		&ret_uint,
		&ret_size);
	return (unsigned int)ret_uint;
}

unsigned long GetDeviceInfoULong(
	cl_device_id device_id,
	cl_device_info device_info)
{
	cl_ulong ret_ulong;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		device_info,
		sizeof(cl_ulong),
		&ret_ulong,
		&ret_size);
	return (unsigned long)ret_ulong;
}

size_t GetDeviceInfoSizeT(
	cl_device_id device_id,
	cl_device_info device_info)
{
	size_t ret_size1;
	size_t ret_size2;
	cl_int err = clGetDeviceInfo(
		device_id,
		device_info,
		sizeof(size_t),
		&ret_size1,
		&ret_size2);
	return (size_t)ret_size1;
}
/*
std::string GetDeviceInfoFPConfig(
	cl_device_id device_id,
	cl_device_info device_info)
{
	cl_device_fp_config device_fp_config;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_DOUBLE_FP_CONFIG,
		sizeof(cl_device_fp_config),
		&device_fp_config,
		&ret_size);
	std::string flags = "";
	if (device_fp_config & CL_FP_DENORM)
		flags += std::string("CL_FP_DENORM ");
	if (device_fp_config & CL_FP_INF_NAN)
		flags += std::string("CL_FP_INF_NAN ");
	if (device_fp_config & CL_FP_ROUND_TO_NEAREST)
		flags += std::string("CL_ROUND_TO_NEAREST ");
	if (device_fp_config & CL_FP_ROUND_TO_ZERO)
		flags += std::string("CL_ROUND_TO_ZERO ");
	if (device_fp_config & CL_FP_ROUND_TO_INF)
		flags += std::string("CL_ROUND_TO_INF ");
	if (device_fp_config & CL_FP_FMA)
		flags += std::string("CL_FP_FMA ");
	return flags;
}
*/
std::string GetDeviceInfoExecutionCapabilities(
	cl_device_id device_id)
{
	cl_device_exec_capabilities device_exec_capabilities;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_EXECUTION_CAPABILITIES,
		sizeof(cl_device_exec_capabilities),
		&device_exec_capabilities,
		&ret_size);
	std::string flags = "";
	if (device_exec_capabilities & CL_EXEC_KERNEL)
		flags += std::string("CL_EXEC_KERNEL");
	if (device_exec_capabilities & CL_EXEC_NATIVE_KERNEL)
		flags += std::string("CL_EXEC_NATIVE_KERNEL");
	return flags;
}

std::string GetDeviceInfoGlobalMemCacheType(
	cl_device_id device_id)
{
	cl_device_mem_cache_type device_mem_cache_type;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
		sizeof(cl_device_mem_cache_type),
		&device_mem_cache_type,
		&ret_size);
	switch (device_mem_cache_type) {
		case CL_NONE : 
			return std::string("CL_NONE");
		case CL_READ_ONLY_CACHE : 
			return std::string("CL_READ_ONLY_CACHE");
		case CL_READ_WRITE_CACHE : 
			return std::string("CL_READ_WRITE_CACHE");
		default :
			return std::string("default");
	}
}

std::string GetDeviceInfoLocalMemType(
	cl_device_id device_id)
{	
	cl_device_local_mem_type device_local_mem_type;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
		sizeof(cl_device_local_mem_type),
		&device_local_mem_type,
		&ret_size);
	switch (device_local_mem_type) {
		case CL_LOCAL :
			return std::string("CL_LOCAL");
		case CL_GLOBAL :
			return std::string("CL_GLOBAL");
		default :
			return std::string("default");
	}
}

std::string GetDeviceInfoMaxWorkItemSizes(
	cl_device_id device_id)
{
	cl_uint dim = GetDeviceInfoUInt(
		device_id, 
		CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
	size_t ret_size = 0;
	size_t* p = new size_t[dim];
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_MAX_WORK_ITEM_SIZES,
		sizeof(size_t) * dim,
		p,
		&ret_size);
	std::stringstream ss("");
	ss << "(";
	for (int i = 0; i < dim; ++i) {
		if (i != 0) ss << ", ";
		ss << p[i];
	}
	ss << ")";
	return ss.str();
}

cl_platform_id GetDeviceInfoPlatformID(
	cl_device_id device_id)
{
	cl_platform_id platform_id;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_PLATFORM,
		sizeof(cl_platform_id),
		&platform_id,
		&ret_size);
	return platform_id;
}

std::string GetDeviceInfoQueueProperties(
	cl_device_id device_id)
{
	cl_command_queue_properties command_queue_properties;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_QUEUE_PROPERTIES,
		sizeof(cl_command_queue_properties),
		&command_queue_properties,
		&ret_size);
	switch (command_queue_properties) {
		case CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE :
			return std::string("CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
		case CL_QUEUE_PROFILING_ENABLE :
			return std::string("CL_QUEUE_PROFILING_ENABLE");
		default :
			return std::string("default?");
	}
}

std::string GetDeviceInfoSingleFPConfig(
	cl_device_id device_id) 
{
	cl_device_fp_config device_fp_config;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_SINGLE_FP_CONFIG,
		sizeof(cl_device_fp_config),
		&device_fp_config,
		&ret_size);
	std::string flags = "";
	if (device_fp_config & CL_FP_DENORM)
		flags += "CL_FP_DENORM ";
	if (device_fp_config & CL_FP_INF_NAN)
		flags += "CL_FP_INF_NAN ";
	if (device_fp_config & CL_FP_ROUND_TO_NEAREST)
		flags += "CL_FP_ROUND_TO_NEAREST ";
	if (device_fp_config & CL_FP_ROUND_TO_ZERO)
		flags += "CL_FP_ROUND_TO_ZERO ";
	if (device_fp_config & CL_FP_ROUND_TO_INF)
		flags += "CL_FP_ROUND_TO_INF ";
	if (device_fp_config & CL_FP_FMA)
		flags += "CL_FP_FMA ";
	if (device_fp_config & CL_FP_INF_NAN)
		flags += "CL_FP_INF_NAN ";
	return flags;
}

std::string GetDeviceInfoType(
	cl_device_id device_id)
{
	cl_device_type device_type;
	size_t ret_size = 0;
	cl_int err = clGetDeviceInfo(
		device_id,
		CL_DEVICE_TYPE,
		sizeof(cl_device_type),
		&device_type,
		&ret_size);
	switch (device_type) {
		case CL_DEVICE_TYPE_CPU :
			return std::string("CL_DEVICE_TYPE_CPU");
		case CL_DEVICE_TYPE_GPU :
			return std::string("CL_DEVICE_TYPE_GPU");
		case CL_DEVICE_TYPE_ACCELERATOR :
			return std::string("CL_DEVICE_TYPE_ACCELERATOR");
		case CL_DEVICE_TYPE_DEFAULT :
			return std::string("CL_DEVICE_TYPE_DEFAULT");
		default :
			return std::string("default?");
	}
}

#define PRINT_DEVICE_INFO_STRING(id, info) { \
	std::cout << #info << " \t:\t" \
		<< GetDeviceInfoString(id, info) << std::endl; \
}

#define PRINT_PLATFORM_INFO_STRING(id, info) { \
	std::cout << "\t" << #info << " \t:\t" \
		<< GetPlatformInfoString(id, info) << std::endl; \
}

#define PRINT_DEVICE_INFO_BOOL(id, info) { \
	std::cout << #info << " \t:\t" \
		<< ((GetDeviceInfoBool(id, info)) ? "true" : "false") \
		<< std::endl; \
}

#define PRINT_DEVICE_INFO_UINT(id, info) { \
	std::cout << #info << " \t:\t" \
		<< GetDeviceInfoUInt(id, info) << std::endl; \
}

#define PRINT_DEVICE_INFO_ULONG(id, info) { \
	std::cout << #info << " \t:\t" \
		<< GetDeviceInfoULong(id, info) << std::endl; \
}

#define PRINT_DEVICE_INFO_SIZE_T(id, info) { \
	std::cout << #info << " \t:\t" \
		<< GetDeviceInfoSizeT(id, info) << std::endl; \
}
/*
void PRINT_DEVICE_DOUBLE_FP_CONFIG(cl_device_id id) { 
	std::cout << "CL_DEVICE_DOUBLE_FP_CONFIG" << " \t:\t" 
		<< GetDeviceInfoFPConfig(id, CL_DEVICE_DOUBLE_FP_CONFIG) 
		<< std::endl; 
}
*/
void PRINT_DEVICE_EXECUTION_CAPABILITIES(cl_device_id id) { 
	std::cout << "CL_DEVICE_ECECUTION_CAPABILITIES" << " \t:\t" 
		<< GetDeviceInfoExecutionCapabilities(id) << std::endl; 
}

void PRINT_DEVICE_GLOBAL_MEM_CACHE_TYPE(cl_device_id id) { 
	std::cout << "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE" << " \t:\t" 
		<< GetDeviceInfoGlobalMemCacheType(id) << std::endl; 
}
/*
void PRINT_DEVICE_HALF_FP_CONFIG(cl_device_id id) { 
	std::cout << "CL_DEVICE_HALF_FP_CONFIG" << " \t:\t" 
		<< GetDeviceInfoFPConfig(id, CL_DEVICE_HALF_FP_CONFIG) 
		<< std::endl; 
}
*/
void PRINT_DEVICE_LOCAL_MEM_TYPE(cl_device_id id) { 
	std::cout << "CL_DEVICE_LOCAL_MEM_TYPE" << " \t:\t" 
		<< GetDeviceInfoLocalMemType(id) << std::endl; 
}

void PRINT_DEVICE_MAX_WORK_ITEM_SIZES(cl_device_id id) { 
	std::cout << "CL_DEVICE_MAX_WORK_ITEM_SIZES" << " \t:\t" 
		<< GetDeviceInfoMaxWorkItemSizes(id) << std::endl; 
}

void PRINT_DEVICE_PLATFORM(cl_device_id id) {
	std::cout << "CL_DEVICE_PLATFORM" << std::endl;
	cl_platform_id platform_id = GetDeviceInfoPlatformID(id);
	PRINT_PLATFORM_INFO_STRING(platform_id, CL_PLATFORM_PROFILE);	
	PRINT_PLATFORM_INFO_STRING(platform_id, CL_PLATFORM_VERSION);
	PRINT_PLATFORM_INFO_STRING(platform_id, CL_PLATFORM_NAME);
	PRINT_PLATFORM_INFO_STRING(platform_id, CL_PLATFORM_VENDOR);
	PRINT_PLATFORM_INFO_STRING(platform_id, CL_PLATFORM_EXTENSIONS);
}

void PRINT_DEVICE_QUEUE_PROPERTIES(cl_device_id id) {
	std::cout << "CL_DEVICE_QUEUE_PROPERTIES" << " \t:\t"
		<< GetDeviceInfoQueueProperties(id) << std::endl;
}

void PRINT_DEVICE_SINGLE_FP_CONFIG(cl_device_id id) {
	std::cout << "CL_DEVICE_SINGLE_FP_CONFIG" << " \t:\t"
		<< GetDeviceInfoSingleFPConfig(id) << std::endl;
}
void PRINT_DEVICE_TYPE(cl_device_id id) {
	std::cout << "CL_DEVICE_TYPE" << " \t:\t"
		<< GetDeviceInfoType(id) << std::endl;
}

void print_device_info(cl_device_id device_id) {
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_ADDRESS_BITS);
	PRINT_DEVICE_INFO_BOOL(device_id, CL_DEVICE_AVAILABLE);
	PRINT_DEVICE_INFO_BOOL(device_id, CL_DEVICE_COMPILER_AVAILABLE);
//	PRINT_DEVICE_DOUBLE_FP_CONFIG(device_id);
	PRINT_DEVICE_INFO_BOOL(device_id, CL_DEVICE_ENDIAN_LITTLE);
	PRINT_DEVICE_INFO_BOOL(device_id, CL_DEVICE_ERROR_CORRECTION_SUPPORT);
	PRINT_DEVICE_EXECUTION_CAPABILITIES(device_id);
	PRINT_DEVICE_INFO_STRING(device_id, CL_DEVICE_EXTENSIONS);
	PRINT_DEVICE_INFO_ULONG(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
	PRINT_DEVICE_GLOBAL_MEM_CACHE_TYPE(device_id);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
	PRINT_DEVICE_INFO_ULONG(device_id, CL_DEVICE_GLOBAL_MEM_SIZE);
//	PRINT_DEVICE_HALF_FP_CONFIG(device_id);
	PRINT_DEVICE_INFO_BOOL(device_id, CL_DEVICE_IMAGE_SUPPORT);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_IMAGE2D_MAX_HEIGHT);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_IMAGE2D_MAX_WIDTH);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_IMAGE3D_MAX_DEPTH);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_IMAGE3D_MAX_HEIGHT);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_IMAGE3D_MAX_WIDTH);
	PRINT_DEVICE_INFO_ULONG(device_id, CL_DEVICE_LOCAL_MEM_SIZE);
	PRINT_DEVICE_LOCAL_MEM_TYPE(device_id);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MAX_COMPUTE_UNITS);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MAX_CONSTANT_ARGS);
	PRINT_DEVICE_INFO_ULONG(device_id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
	PRINT_DEVICE_INFO_ULONG(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_MAX_PARAMETER_SIZE);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MAX_READ_IMAGE_ARGS);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MAX_SAMPLERS);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
	PRINT_DEVICE_MAX_WORK_ITEM_SIZES(device_id);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MEM_BASE_ADDR_ALIGN);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
	PRINT_DEVICE_INFO_STRING(device_id, CL_DEVICE_NAME);
	PRINT_DEVICE_PLATFORM(device_id);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
	PRINT_DEVICE_INFO_STRING(device_id, CL_DEVICE_PROFILE);
	PRINT_DEVICE_INFO_SIZE_T(device_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION);
	PRINT_DEVICE_QUEUE_PROPERTIES(device_id);
	PRINT_DEVICE_SINGLE_FP_CONFIG(device_id);
	PRINT_DEVICE_TYPE(device_id);
	PRINT_DEVICE_INFO_STRING(device_id, CL_DEVICE_VENDOR);
	PRINT_DEVICE_INFO_UINT(device_id, CL_DEVICE_VENDOR_ID);
	PRINT_DEVICE_INFO_STRING(device_id, CL_DEVICE_VERSION);
	PRINT_DEVICE_INFO_STRING(device_id, CL_DRIVER_VERSION);
}

int enum_platform(cl_platform_id platform_id) {

	cl_uint num_devices = 0;
	{
		// check the number of devices
		cl_int err = clGetDeviceIDs(
			platform_id,
			CL_DEVICE_TYPE_ALL,
			0,
			NULL,
			&num_devices);
	}
	{
		printf("found(%d) devices.\n", num_devices);
		// declare memory to hold the device id
		cl_device_id* device_ids = new cl_device_id[num_devices];
	
		// ask OpenCL for exactly 1 GPU
		cl_int err = clGetDeviceIDs(
			platform_id,
			CL_DEVICE_TYPE_ALL,
			num_devices,
			device_ids,
			NULL);
	
		// make sure nothing went wrong
		if (err != CL_SUCCESS) {
			printf("Error(%d) in creating device!\n", err);
			switch (err) {
				case CL_INVALID_PLATFORM :
					printf("\tCL_INVALID_PLATFORM\n");
					break;
				case CL_INVALID_DEVICE_TYPE :
					printf("\tCL_INVALID_DEVICE_TYPE\n");
					break;
				case CL_INVALID_VALUE :
					printf("\tCL_INVALID_VALUE\n");
					break;
				case CL_DEVICE_NOT_FOUND :
					printf("\tCL_DEVICE_NOT_FOUND\n");
					break;
			}
			return EXIT_FAILURE;
		}
		for (int i = 0; i < num_devices; ++i)
			print_device_info(device_ids[i]);
	}
	return EXIT_SUCCESS;	
}

int main(int ac, char** av) {
	cl_uint num_platforms = 0;
	{
		cl_int err = clGetPlatformIDs(
			0,
			NULL,
			&num_platforms);
		if (err != CL_SUCCESS) {
			printf("error(%d) in enumerating platform!\n", err);
			return EXIT_FAILURE;
		}
	}
	{
		printf("found(%d) platforms.\n", num_platforms);
		cl_platform_id* platform_ids = new cl_platform_id[num_platforms];
		cl_int err = clGetPlatformIDs(
			num_platforms,
			platform_ids,
			NULL);
		for (int i = 0; i < num_platforms; ++i)
			if (enum_platform(platform_ids[i]) != EXIT_SUCCESS)
				return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}	

