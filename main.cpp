#include "GL/glew.h"
#define GLFW_DLL
#include <GLFW/glfw3.h>


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


#include <CL/cl.h>


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
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
    
    size_t local;
};

OpenCLData init_cl(World *world)
{
    int err;
    
    //size_t global;
    //size_t local;
    
    OpenCLData cl;
    
    char *kernelFile = "raytracer-kernel.c";
    char *kernelSource = readFile(kernelFile);
    
    
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
		cl_platform_id* platforms = 
            (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
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
    
    // create compute context
    cl.context = clCreateContext(NULL, 1, &cl.device_id, NULL, NULL, &err);
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
    cl.inputRays = clCreateBuffer(cl.context, CL_MEM_READ_ONLY, sizeof(Ray) * SCREEN_WIDTH * SCREEN_HEIGHT, NULL, NULL);
    cl.outputPixels = clCreateBuffer(cl.context, CL_MEM_WRITE_ONLY, sizeof(Vec3) * SCREEN_WIDTH * SCREEN_HEIGHT, NULL, NULL);
    
    if(!cl.inputWorld || !cl.outputPixels || !cl.inputRays) {
        printf("Error: Failed to create memory buffers\n");
        exit(1);
    }
    
    err = clEnqueueWriteBuffer(cl.commands, cl.inputWorld, CL_TRUE, 0, sizeof(World), world, 0, NULL, NULL);
    //err = clEnqueueWriteBuffer(cl.commands, cl.inputRays, CL_TRUE, 0, sizeof(Ray) * SCREEN_WIDTH * SCREEN_HEIGHT, rays, 0, NULL, &evWriteBuf);
    
    // TODO(NAME): set kernel arguments
    err = 0;
    err = clSetKernelArg(cl.kernel, 0, sizeof(cl_mem), (void*)&cl.inputWorld);
    err |= clSetKernelArg(cl.kernel, 1, sizeof(cl_mem), (void*)&cl.inputRays);
    err |= clSetKernelArg(cl.kernel, 2, sizeof(cl_mem), (void*)&cl.outputPixels);
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
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    
    cam = CAMERA(VEC3(0, 0, 0), VEC3(0, 0, -1), VEC3(0, 1, 0), 0.785);
    camPitch = asin(cam.g.y);
    camYaw = atan2(cam.g.x, cam.g.z);
    
    //glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
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
    
    Ray *rays = (Ray*)malloc(sizeof(Ray) * SCREEN_WIDTH * SCREEN_HEIGHT);
    //Vec3 *rayD = (Vec3*)malloc(sizeof(Vec3) * SCREEN_WIDTH * SCREEN_HEIGHT);
    //Vec3 *rayO = (Vec3*)malloc(sizeof(Vec3) * SCREEN_WIDTH * SCREEN_HEIGHT);
    //Ray rays[480000];
    OpenCLData cl = init_cl(&world);
    
    cl_event evKernel, evReadBuf, evWriteBuf;
    int err = 0;
    
    bool renderOnce = false;
    while (!glfwWindowShouldClose(win))
    {
        
        //if(glfwGetTime() > moveStartTimer + moveDuration) {
        //moving = false;
        //}
        
        glfwPollEvents();
        
        //ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // add imgui code here
        bool show = true;
        ImGui::Begin("test", &show);
        ImGui::Text("this is a test window");
        ImGui::End();
        
        int i = 0;
        for(int y = 0; y < SCREEN_HEIGHT; y++) {
            for(int x = 0; x < SCREEN_WIDTH; x++) {
                int yFlipped = SCREEN_HEIGHT - 1 - y;
                rays[i] = RayFor(cam, SCREEN_WIDTH, SCREEN_HEIGHT, x, yFlipped);
                //rays[i] = RAY(VEC3(0,0,0), VEC3(0,0,-1));
                i++;
            }
        }
        
        err = clEnqueueWriteBuffer(cl.commands, cl.inputRays, CL_TRUE, 0, sizeof(Ray) * SCREEN_WIDTH * SCREEN_HEIGHT, rays, 0, NULL, &evWriteBuf);
        if(err != CL_SUCCESS) {
            printf("Error: Failed to write to inputWorld %d\n", err);
            exit(1);
        }
        // Execute the kernel over the entire range of our 1d input data set
        // using the maximum number of work group items for this device
        //
        size_t global = SCREEN_HEIGHT * SCREEN_WIDTH;
        //size_t global = 240000;
        err = clEnqueueNDRangeKernel(cl.commands, cl.kernel, 1, NULL, &global, &cl.local, 0, NULL, &evKernel);
        if (err)
        {
            printf("Error: Failed to execute kernel!\n");
            return EXIT_FAILURE;
        }
        //clWaitForEvents(1, &evKernel);
        // Wait for the command commands to get serviced before reading back results
        clFinish(cl.commands);
        // Read back the results from the device to verify the output
        err = clEnqueueReadBuffer( cl.commands, cl.outputPixels, CL_TRUE, 0, sizeof(Vec3) * SCREEN_WIDTH * SCREEN_HEIGHT, &(pixelData[0]), 0, NULL, NULL );  
        if (err != CL_SUCCESS)
        {
            printf("Error: Failed to read output array! %d\n", err);
            exit(1);
        }
        
        
#if 0        
        if(!renderOnce) {
            for(int i = 0; i < SCREEN_WIDTH *  SCREEN_HEIGHT; i++) {
                pixelData[i] = WorldHit(&world, rays[i]);
            }
            renderOnce = true;
        }
#endif
        
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_FLOAT, &pixelData[0]);
        glBindTexture(GL_TEXTURE_2D, 0);
        
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
    }
    
    
    free(rays);
    cleanup_cl(&cl);
    free(world.planes);
    free(world.spheres);
    free(pixelData);
    glfwTerminate();
    
    return 0;
}
