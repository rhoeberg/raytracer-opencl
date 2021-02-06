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


#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])
#define KILOBYTES(value) ((value)*1024)
#define MEGABYTES(value) (KILOBYTES(value) * 1024)
#define GIGABYTES(value) (GIGABYTES(value) * 1024)

#define MOUSE_SENSITIVITY 0.1
#define EPSILON 0.0001

#include <iostream>
#include <CL/cl.h>
#include <CL/cl_gl.h>


#include "math.h"
#include "camera.h"
#include "world.h"

#include "light.cpp"
#include "camera.cpp"
#include "math.cpp"
#include "geometry.cpp"
#include "raytracer.cpp"
#include "world.cpp"


#if 0
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif


#define CHECK_ERR(E) if(E != CL_SUCCESS) fprintf (stderr, "CL ERROR (%d) in %s:%d\n", E,__FILE__, __LINE__);
#define CHECK_GL(C) C; do {GLenum glerr = glGetError(); if(glerr != GL_NO_ERROR) printf("GL ERROR (%d) in %s:%d\n", glerr, __FILE__, __LINE__);} while(0)

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TOTAL_PIXELS (SCREEN_WIDTH * SCREEN_HEIGHT)

bool keys[1024];
bool registeredKeys[1024];

static bool firstMouse = false;
static float lastX;
static float lastY;
static float mouseX;
static float mouseY;

static float camYaw;
static float camPitch;
static Camera cam;

static bool moving = true;
static bool movedLastFrame = true;
float moveStartTimer = 0.0f;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (action == GLFW_PRESS)
    {
        keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        keys[key] = false;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;
    
    camYaw -= xoffset;
    camPitch += yoffset;
    
    if (camPitch > 89.0f)
        camPitch = 89.0f;
    if (camPitch < -89.0f)
        camPitch = -89.0f;
    
    Vec3 dir;
    dir.x = cos(RADIANS(camYaw)) * cos(RADIANS(camPitch));
    dir.y = sin(RADIANS(camPitch));
    dir.z = sin(RADIANS(camYaw)) * cos(RADIANS(camPitch));
    
    cam.g = Norm(dir);
    cam.w = cam.g * -1;
    cam.u = Norm(Cross(cam.t, cam.w));
    cam.v = Cross(cam.w, cam.u);
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

char *readFile(const char *fileName)
{
    char *result = 0;
    FILE *file = fopen(fileName, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        result = (char *)malloc(fileSize + 1);
        fread(result, fileSize, 1, file);
        result[fileSize] = 0;
        
        fclose(file);
    }
    else
    {
        printf("cannot find file %s\n", fileName);
    }
    
    return result;
}

GLuint create_shader(const char *vPath, const char *fPath)
{
    GLuint shaderProgram;
    const char *vertexCode = readFile(vPath);
    const char *fragmentCode = readFile(fPath);
    
    GLint success;
    GLchar infoLog[512];
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
        printf("%s\n", infoLog);
    }
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

void init_glfw(GLFWwindow **win)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    *win = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "great concept", NULL, NULL);
    if (!win)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(*win);
    
    glfwSetCursorPosCallback(*win, mouse_callback);
    glfwSetKeyCallback(*win, key_callback);
    
    // Set this to true so glew knows to use modern opengl
    glewExperimental = GL_TRUE;
    glewInit();
    
    int vWidth, vHeight;
    glfwGetFramebufferSize(*win, &vWidth, &vHeight);
    glViewport(0, 0, vWidth, vHeight);
    
    // enable depths testing to remove pixels which is behind other pixels
    glEnable(GL_DEPTH_TEST);
}

void create_quad(GLuint *VAO)
{
    GLfloat vertices[] = {
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };
    GLuint indices[] = {
        0, 1, 2,
        0, 3, 1};
    
    // GLuint VBO, VAO, EBO;
    GLuint VBO, EBO;
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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


/**
* Selects CL platform/device capable of CL/GL interop.
*/
void cl_select(cl_platform_id* platform_id, cl_device_id* device_id) {
    cl_int err;
    int i;
    char* info;
    size_t infoSize;
    cl_uint platformCount;
    cl_platform_id *platforms;
    
    // get platform count
    err = clGetPlatformIDs(5, NULL, &platformCount);
    CHECK_ERR(err);
    
    // get all platforms
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    err = clGetPlatformIDs(platformCount, platforms, NULL);
    CHECK_ERR(err);
    
    // for each platform print all attributes
    for (i = 0; i < platformCount; i++) {
        
        printf("%d. Checking Platform \n", i+1);
        
        // get platform attribute value size
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 0, NULL, &infoSize);
        CHECK_ERR(err);
        info = (char*) malloc(infoSize);
        
        // get platform attribute value
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, infoSize, info, NULL);
        CHECK_ERR(err);
        
        if(strstr(info, GL_SHARING_EXTENSION) != NULL) {
            cl_uint num_devices;
            cl_device_id* devices;
            
            // Get the number of GPU devices available to the platform
            err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
            CHECK_ERR(err);
            
            // Create the device list
            devices = new cl_device_id [num_devices];
            err  = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
            CHECK_ERR(err);
            
            int d;
            for(d = 0; d < num_devices; d++) {
                
                // get device attribute value size
                size_t extensionSize;
                err = clGetDeviceInfo(devices[d], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize );
                CHECK_ERR(err);
                
                if(extensionSize > 0) {
                    char* extensions = (char*)malloc(extensionSize);
                    err = clGetDeviceInfo(devices[d], CL_DEVICE_EXTENSIONS, extensionSize, extensions, &extensionSize);
                    CHECK_ERR(err);
                    
                    if(strstr(info, GL_SHARING_EXTENSION) != NULL) {
                        printf("Found Compatible platform %d and device %d out of %d .\n", i, d, num_devices);
                        *platform_id = platforms[i];
                        *device_id = devices[d];
                        
                        // TODO remove. currently a toggle for intel/nvidia platform
                        // break;
                    }
                    
                    free(extensions);
                }
                
            }
        }
        
        free(info);
        printf("\n");
        
    }
    
    free(platforms);
}


void cl_select_context(cl_platform_id* platform, cl_device_id* device, cl_context* context) {
    cl_int err;
#if defined (__APPLE__)
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    cl_context_properties props[] =
    {
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
        0
    };
    *context = clCreateContext(props, 0,0, NULL, NULL, &err);
#else
#ifdef UNIX
    cl_context_properties props[] =
    {
        CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
        0
    };
    *context = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &err);
#else // Win32
    cl_context_properties props[] =
    {
        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)*platform,
        0
    };
    *context = clCreateContext(props, 1, device, NULL, NULL, &err);
    CHECK_ERR(err);
#endif
#endif
}


void cl_load_kernel(cl_context* context, cl_device_id* device, const char* source, cl_command_queue* command_queue, cl_kernel* kernel, World *world, cl_mem *inputWorld, cl_mem *outputDebug) {
    
    cl_int err;
    cl_program program;
    char *source_str = readFile(source);;
    
#if 0
    cl_mem memobj;
    char string[MEM_SIZE];
    
    FILE *fp;
    size_t source_size;
    
    /* Load the source code containing the kernel*/
    fp = fopen(source, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *) malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);
#endif
    
    
    // create a command queue
    *command_queue = clCreateCommandQueue(*context, *device, 0, &err);
    CHECK_ERR(err);
    
    
    //memobj = clCreateBuffer(*context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(char), NULL, &err);
    //CHECK_ERR(err);
    
    *inputWorld = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(World), NULL, &err);
    CHECK_ERR(err);
    *outputDebug = clCreateBuffer(*context, CL_MEM_WRITE_ONLY, sizeof(Ray), NULL, &err);
    CHECK_ERR(err);
    CHECK_ERR(clEnqueueWriteBuffer(*command_queue, *inputWorld, CL_TRUE, 0, sizeof(World), world, 0, NULL, NULL));
    
    /* Create Kernel Program from the sour
    ce */
    program = clCreateProgramWithSource(*context, 1, (const char **) &source_str,
                                        (const size_t *) NULL, &err);
    CHECK_ERR(err);
    
    
    /* Build Kernel Program */
    err = clBuildProgram(program, 1, device, NULL, NULL, NULL);
    if(err != CL_SUCCESS) {
        size_t len;
        cl_build_status build_status;
        char buffer[204800];
        err = clGetProgramBuildInfo(program, *device, CL_PROGRAM_BUILD_STATUS, sizeof(build_status), (void *)&build_status, &len);
        CHECK_ERR(err);
        err = clGetProgramBuildInfo(program, *device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        CHECK_ERR(err);
        printf("Build Log:\n%s\n", buffer);
        exit(1);
    }
    
    /* Create OpenCL Kernel */
    *kernel = clCreateKernel(program, "WorldHitKernel", &err);
    CHECK_ERR(err);
}

OpenCLData init_cl(World *world, GLFWwindow *win)
{
    int err;
    
    //size_t global;
    //size_t local;
    
    OpenCLData cl;
    
    char *kernelFile = "raytracer-kernel.c";
    char *kernelSource = readFile(kernelFile);
    
    
#if 0    
	/*Step1: Getting platforms and choose an available one.*/
	cl_uint numPlatforms; //the NO. of platforms
	cl_platform_id platform = NULL; //the chosen platform
	cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
        printf("Error: Getting platforms!");
		exit(1);
	}
    
	/*For clarity, choose the first available platform. */
    if (numPlatforms > 0)
	{
        cl_platform_id *platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}
    
    // connect to compute device
    int gpu = 1;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &cl.device_id, NULL);
    if(err != CL_SUCCESS){
        printf("Error: Failed to create a device group!\n");
        exit(1);
    }
    
    // set CL/GL context
    cl_context_properties props[] =
    {
        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
        0
    };
    
    // create compute context
    cl.context = clCreateContext(props, 1, &cl.device_id, NULL, NULL, &err);
    if(!cl.context) {
        printf("Error: failed to create a compute context!\n");
        exit(1);
    }
    
    // create commands
    cl.commands = clCreateCommandQueue(cl.context, cl.device_id, 0, &err);
    if(!cl.commands) {
        printf("Error: failed to create a command commands!\n");
        exit(1);
    }
    
    // create compute program from source buffer
    cl.program = clCreateProgramWithSource(cl.context, 1, (const char**) &kernelSource, NULL, &err);
    if(!cl.program) {
        printf("Error: Failed to create compute program!\n");
        exit(1);
    }
    
    // build program executable
    err = clBuildProgram(cl.program, 1, &cl.device_id, NULL, NULL, NULL);
    if(err != CL_SUCCESS) {
        size_t len;
        char buffer[65536];
        
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(cl.program, cl.device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), &buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
    
    // create compute kernel
    cl.kernel = clCreateKernel(cl.program, "WorldHitKernel", &err);
    if(!cl.kernel || err != CL_SUCCESS) {
        printf("Error: Failed to create compute kernel!\n");
        exit(1);
    }
    
    // TODO(NAME): create memory buffers here
    cl.inputWorld = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(World), NULL, NULL);
    //cl.inputRays = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(Ray) * SCREEN_WIDTH * SCREEN_HEIGHT, NULL, NULL);
    //cl.outputPixels = clCreateBuffer(cl.context, CL_MEM_WRITE_ONLY, sizeof(Vec3) * SCREEN_WIDTH * SCREEN_HEIGHT, NULL, NULL);
    cl.outputDebug = clCreateBuffer(cl.context, CL_MEM_WRITE_ONLY, sizeof(Ray), NULL, NULL);
    
    if(!cl.inputWorld) {
        printf("Error: Failed to create memory buffers\n");
        exit(1);
    }
    
#endif
    
    cl_platform_id platform = NULL;
    cl_select(&platform, &cl.device_id);
    cl_select_context(&platform, &cl.device_id, &cl.context);
    cl_load_kernel(&cl.context, &cl.device_id, kernelFile, &cl.commands, &cl.kernel, world, &cl.inputWorld, &cl.outputDebug);
    
    err = clEnqueueWriteBuffer(cl.commands, cl.inputWorld, CL_TRUE, 0, sizeof(World), world, 0, NULL, NULL);
    //err = clEnqueueWriteBuffer(cl.commands, cl.inputRays, CL_TRUE, 0, sizeof(Ray) * SCREEN_WIDTH * SCREEN_HEIGHT, rays, 0, NULL, &evWriteBuf);
    
    // TODO(NAME): set kernel arguments
    err = 0;
    err = clSetKernelArg(cl.kernel, 0, sizeof(cl_mem), (void*)&cl.inputWorld);
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    err |= clSetKernelArg(cl.kernel, 1, sizeof(int), &width);
    err |= clSetKernelArg(cl.kernel, 2, sizeof(int), &height);
    //err |= clSetKernelArg(cl.kernel, 3, sizeof(cl_mem), (void*)&cl.outputPixels);
    err |= clSetKernelArg(cl.kernel, 4, sizeof(cl_mem), (void*)&cl.outputDebug);
    if(err != CL_SUCCESS) {
        printf("Error: Failed to set kernel arguments! %d\n", err);
        exit(1);
    }
    
    
    // Get the maximum work group size for executing the kernel on the device
    //
    err = clGetKernelWorkGroupInfo(cl.kernel, cl.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(cl.local), &cl.local, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        exit(1);
    }
    
    
    // cleanup
    free(kernelSource);
    
    return cl;
}

void cleanup_cl(OpenCLData *cl)
{
    clReleaseProgram(cl->program);
    clReleaseKernel(cl->kernel);
    clReleaseCommandQueue(cl->commands);
    clReleaseContext(cl->context);
}

int main(int argc, char *argv[])
{
    
    
    GLFWwindow *win;
    init_glfw(&win);
    
    GLuint quad;
    create_quad(&quad);
    
    Vec3 *pixelData = (Vec3 *)malloc(sizeof(Vec3) * SCREEN_WIDTH * SCREEN_HEIGHT);
    for (int i = 0; i < 480000; i++)
    {
        pixelData[i] = VEC3(0, 0, 0);
    }
    
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    cam = CAMERA(VEC3(0, 0, 0), VEC3(0, 0, -1), VEC3(0, 1, 0), 0.785);
    camPitch = asin(cam.g.y);
    camYaw = atan2(cam.g.x, cam.g.z);
    
    const char* glsl_version = "#version 150";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    World world = InitializeDefaultWorld();
    
    GLuint shaderProgram = create_shader("vertexshader.vs", "fragmentshader.frag");
    
    
    float moveDuration = 0.5f;
    float moveStartTime = 0.0f;
    
    //OpenCLData cl = init_cl(&world, win);
    printf("here first\n");
    char *kernelFile = "raytracer-kernel.c";
    OpenCLData cl;
    cl_platform_id platform = NULL;
    cl_select(&platform, &cl.device_id);
    cl_select_context(&platform, &cl.device_id, &cl.context);
    cl_load_kernel(&cl.context, &cl.device_id, kernelFile, &cl.commands, &cl.kernel, &world, &cl.inputWorld, &cl.outputDebug);
    
    //clEnqueueWriteBuffer(cl.commands, cl.inputWorld, CL_TRUE, 0, sizeof(World), (void*)(&world), 0, NULL, NULL);
    //err = clEnqueueWriteBuffer(cl.commands, cl.inputRays, CL_TRUE, 0, sizeof(Ray) * SCREEN_WIDTH * SCREEN_HEIGHT, rays, 0, NULL, &evWriteBuf);
    
    cl.outputTexture = clCreateFromGLTexture(cl.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture, NULL);
    if(!cl.outputTexture) {
        printf("Error: failed to create texture buffer\n");
        exit(1);
    }
    
    //glFinish();
    //clEnqueueAcquireGLObjects(cl.commands, 1, &cl.outputTexture, 0, 0, NULL);
    CHECK_ERR(clSetKernelArg(cl.kernel, 0, sizeof(cl_mem), (void*)&cl.inputWorld));
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    CHECK_ERR(clSetKernelArg(cl.kernel, 1, sizeof(int), &width));
    CHECK_ERR(clSetKernelArg(cl.kernel, 2, sizeof(int), &height));
    CHECK_ERR(clSetKernelArg(cl.kernel, 3, sizeof(cl_mem), &cl.outputTexture));
    CHECK_ERR(clSetKernelArg(cl.kernel, 4, sizeof(cl_mem), (void*)&cl.outputDebug));
    
    
    int err = 0;
    cl_event evKernel, evReadBuf, evWriteBuf;
    
    double frameDuration;
    
    bool renderOnce = false;
    while (!glfwWindowShouldClose(win))
    {
        double frameStart = glfwGetTime();
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool show = true;
        ImGui::Begin("test", &show);
        ImGui::Text("frameduration: %f", frameDuration);
        ImGui::End();
        
        float t = glfwGetTime();
        clSetKernelArg(cl.kernel, 5, sizeof(float), &t);
        
        
        glFinish();
        clEnqueueAcquireGLObjects(cl.commands, 1, &cl.outputTexture, 0, 0, NULL);
        
        // Execute the kernel over the entire range of our 1d input data set
        // using the maximum number of work group items for this device
        //
        size_t global[] = {SCREEN_WIDTH, SCREEN_HEIGHT};
        err = clEnqueueNDRangeKernel(cl.commands, cl.kernel, 2, NULL, global, NULL, 0, 0, 0);
        if (err)
        {
            printf("Error: Failed to execute kernel!\n");
            return EXIT_FAILURE;
        }
        
        // Wait for the command commands to get serviced before reading back results
        clEnqueueReleaseGLObjects(cl.commands, 1, &cl.outputTexture, 0, 0, NULL);
        clFinish(cl.commands);
        
#if 0   
        // CPU RENDER     
        if(!renderOnce) {
            int i = 0;
            for(int y = 0; y < SCREEN_HEIGHT; y++) {
                for(int x = 0; x < SCREEN_WIDTH; x++) {
                    int yFlipped = SCREEN_HEIGHT - 1 - y;
                    Ray ray = RayFor(cam, SCREEN_WIDTH, SCREEN_HEIGHT, x, yFlipped);
                    pixelData[i] = WorldHit(&world, ray);
                    i++;
                }
            }
            renderOnce = true;
        }
#endif
        
        //glBindTexture(GL_TEXTURE_2D, texture);
        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_FLOAT, &pixelData[0]);
        //glBindTexture(GL_TEXTURE_2D, 0);
        
        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glBindVertexArray(quad);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(win);

	frameDuration = glfwGetTime() - frameStart;
    }
    
    
    cleanup_cl(&cl);
    free(world.planes);
    free(world.spheres);
    free(pixelData);
    glfwTerminate();
    
    return 0;
}
