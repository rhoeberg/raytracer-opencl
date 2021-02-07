#include "main.h"

#include "light.cpp"
#include "camera.cpp"
#include "math.cpp"
#include "geometry.cpp"
#include "raytracer.cpp"
#include "world.cpp"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	Input *input = (Input*)glfwGetWindowUserPointer(window);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (action == GLFW_PRESS)
    {
        input->keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->keys[key] = false;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	Input *input = (Input*)glfwGetWindowUserPointer(window);

	input->mouseX = xpos;
	input->mouseY = ypos;

    if (input->firstMouse)
    {
        input->lastX = xpos;
        input->lastY = ypos;
        input->firstMouse = false;
    }
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
    
    *win = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "opencl raytracer by rhoeberg", NULL, NULL);
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

GLuint create_quad()
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
    
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	return VAO;
}

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
    
    // create a command queue
    *command_queue = clCreateCommandQueue(*context, *device, 0, &err);
    CHECK_ERR(err);
    
    *inputWorld = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(World), NULL, &err);
    CHECK_ERR(err);
    *outputDebug = clCreateBuffer(*context, CL_MEM_WRITE_ONLY, sizeof(Ray), NULL, &err);
    CHECK_ERR(err);
    CHECK_ERR(clEnqueueWriteBuffer(*command_queue, *inputWorld, CL_TRUE, 0, sizeof(World), world, 0, NULL, NULL));
    
    /* Create Kernel Program from the source */
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

void cleanup_cl(OpenCLData *cl)
{
    clReleaseProgram(cl->program);
    clReleaseKernel(cl->kernel);
    clReleaseCommandQueue(cl->commands);
    clReleaseContext(cl->context);
}

GLuint create_texture()
{
	GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	return texture;
}

void initialize_opencl(OpenCLData *cl, World *world, GLuint texture)
{
    char *kernelFile = "raytracer-kernel.cl";
    cl_platform_id platform = NULL;
    cl_select(&platform, &cl->device_id);
    cl_select_context(&platform, &cl->device_id, &cl->context);
    cl_load_kernel(&cl->context, &cl->device_id, kernelFile, &cl->commands, &cl->kernel, world, &cl->inputWorld, &cl->outputDebug);
    cl->outputTexture = clCreateFromGLTexture(cl->context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture, NULL);
    if(!cl->outputTexture) {
        printf("Error: failed to create texture buffer\n");
        exit(1);
    }
    CHECK_ERR(clSetKernelArg(cl->kernel, 0, sizeof(cl_mem), (void*)&cl->inputWorld));
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    CHECK_ERR(clSetKernelArg(cl->kernel, 1, sizeof(int), &width));
    CHECK_ERR(clSetKernelArg(cl->kernel, 2, sizeof(int), &height));
    CHECK_ERR(clSetKernelArg(cl->kernel, 3, sizeof(cl_mem), &cl->outputTexture));
    CHECK_ERR(clSetKernelArg(cl->kernel, 4, sizeof(cl_mem), (void*)&cl->outputDebug));
}

int main(int argc, char *argv[])
{
    GLFWwindow *win;
    init_glfw(&win);

	Input input;
	glfwSetWindowUserPointer(win, &input);

    GLuint quad = create_quad();
    GLuint texture = create_texture();
    GLuint shaderProgram = create_shader("vertexshader.vs", "fragmentshader.frag");
    
    Camera cam = CAMERA(VEC3(0, 0, 0), VEC3(0, 0, -1), VEC3(0, 1, 0), 0.785);

	////////////////
	// INITIALIZE IMGUI
    const char* glsl_version = "#version 150";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
	////////////////
	// INITIALIZE WORLD
    World world = InitializeDefaultWorld();
    
	// ////////////////
	// // INITIALIZE OPENCL
	OpenCLData cl;
	initialize_opencl(&cl, &world, texture);
    int err = 0;
    
    double frameDuration = 0;
    while (!glfwWindowShouldClose(win))
    {
        double frameStart = glfwGetTime();
        glfwPollEvents();

		ProcessMouseMovement(&cam, &input);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool show = true;
        ImGui::Begin("performance", &show);
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
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
    glfwTerminate();
    
    return 0;
}
