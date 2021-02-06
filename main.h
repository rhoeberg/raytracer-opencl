#pragma once
#include "Camera.h"

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])
#define KILOBYTES(value) ((value)*1024)
#define MEGABYTES(value) (KILOBYTES(value) * 1024)
#define GIGABYTES(value) (GIGABYTES(value) * 1024)

#define MOUSE_SENSITIVITY 0.1
#define EPSILON 0.0001

// #define SCREEN_WIDTH 1280
// #define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1960
#define SCREEN_HEIGHT 1080

#define CHECK_ERR(E) if(E != CL_SUCCESS) fprintf (stderr, "CL ERROR (%d) in %s:%d\n", E,__FILE__, __LINE__);
#define CHECK_GL(C) C; do {GLenum glerr = glGetError(); if(glerr != GL_NO_ERROR) printf("GL ERROR (%d) in %s:%d\n", glerr, __FILE__, __LINE__);} while(0)


//////////////////////////////
// INCLUDES

#include "GL/glew.h"
#define GLFW_DLL
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#if defined (__APPLE__) || defined(MACOSX)
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif

#include "imgui/imgui.cpp"
#include "imgui/imgui_widgets.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_impl_opengl3.cpp"
#include "imgui/imgui_impl_glfw.cpp"

#include <iostream>
#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "math.h"
#include "camera.h"
#include "world.h"
#include <iostream>
#include <CL/cl.h>
#include <CL/cl_gl.h>


////////////////////////////
// STRUCTS

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

struct ProgramState
{
	bool firstMouse = false;
	float lastX;
	float lastY;
	float mouseX;
	float mouseY;

	float camYaw;
	float camPitch;
	Camera cam;

	bool moving = true;
	bool movedLastFrame = true;
	float moveStartTimer = 0.0f;
};
