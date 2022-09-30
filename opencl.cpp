#include "opencl.h"

// Selects CL platform/device
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
    
	// create buffers
    // *inputWorld = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(World), NULL, &err);
    *inputWorld = clCreateBuffer(*context, CL_MEM_USE_HOST_PTR, sizeof(World), world, &err);
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
}

void cleanup_cl(OpenCLData *cl)
{
	glFinish();
    clFinish(cl->commands);
    clReleaseProgram(cl->program);
    clReleaseKernel(cl->kernel);
    clReleaseCommandQueue(cl->commands);
    clReleaseContext(cl->context);
}
