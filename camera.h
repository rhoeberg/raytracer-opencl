#pragma once
#include "math.h"
#include <CL/cl.h>
#include <CL/cl_gl.h>

#pragma pack(push, 1)
struct Camera
{
	// TODO (rhoe) maybe names these better?
    cl_float3 o; // cam pos
    cl_float3 g; // cam front (gaze direction)
    cl_float3 w; // cam back (negative gaze)
    cl_float3 u; // cam right 
    cl_float3 v; // cam up 
    cl_float3 t; // world up 
    

	float yaw;
	float pitch;
	
    float angle;
	/* Vec3 right; */
    
};
#pragma pack(pop)

