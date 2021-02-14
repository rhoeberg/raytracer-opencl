#pragma once
#include "world.h"

struct OpenCLData
{
	cl_device_id device_id;
	cl_context context;
	cl_command_queue commands;
	cl_program program;
	cl_kernel kernel;
    
	cl_mem inputWorld;
	cl_mem inputRays;
	//cl_mem inputRayD;
	//cl_mem inputRayO;
	cl_mem outputPixels;
	cl_mem outputTexture;
	cl_mem outputDebug;
    
	size_t local;
};

void cl_select(cl_platform_id* platform_id, cl_device_id* device_id);
void cl_select_context(cl_platform_id* platform, cl_device_id* device, cl_context* context);
void cl_load_kernel(cl_context* context, cl_device_id* device, const char* source, cl_command_queue* command_queue, cl_kernel* kernel, World *world, cl_mem *inputWorld, cl_mem *outputDebug);
void initialize_opencl(OpenCLData *cl, World *world, GLuint texture);
void cleanup_cl(OpenCLData *cl);
