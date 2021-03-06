#include "main.h"


//////////////////////
// UNITY BUILD

#include "imgui/imgui.cpp"
#include "imgui/imgui_widgets.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_impl_opengl3.cpp"
#include "imgui/imgui_impl_glfw.cpp"

#include "opencl.cpp"
#include "glfw.cpp"
#include "util.cpp"
#include "opengl.cpp"
#include "light.cpp"
#include "camera.cpp"
#include "math.cpp"
#include "geometry.cpp"
#include "raytracer.cpp"
#include "world.cpp"

int main(int argc, char *argv[])
{
    GLFWwindow *win;
    init_glfw(&win);

	/////////////////
	// INITIALIZE INPUT
	Input input;
	for(int i = 0; i < 1024; i++) {
		input.keys[i] = false;
		input.registeredKeys[i] = false;
	}
	input.firstMouse = true;

	glfwSetWindowUserPointer(win, &input);

    GLuint quad = create_quad();
    GLuint texture = create_texture();
    GLuint shaderProgram = create_shader("vertexshader.vs", "fragmentshader.frag");
    
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
	world.cam = CAMERA({0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 0.785);
    
	// ////////////////
	// // INITIALIZE OPENCL
	OpenCLData cl;
	initialize_opencl(&cl, &world, texture);
    int err = 0;

	cl_ulong size;
	clGetDeviceInfo(cl.device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &size, 0);
	printf("local worker mem size:%llu\n", size);
    
    double frameDuration = 0;
    while (!glfwWindowShouldClose(win))
    {
        double frameStart = glfwGetTime();
        glfwPollEvents();

		ProcessMouseMovement(&world.cam, &input);
		ProcessKeyboard(&world.cam, &input);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool show = true;
        ImGui::Begin("performance", &show);
        ImGui::Text("frameduration: %f", frameDuration);
        ImGui::End();
        

        glFinish();
        clEnqueueAcquireGLObjects(cl.commands, 1, &cl.outputTexture, 0, 0, NULL);

		CHECK_ERR(clEnqueueWriteBuffer(cl.commands, cl.inputWorld, CL_TRUE, 0, sizeof(World), &world, 0, NULL, NULL));

        
        // Execute the kernel over the entire range of our 1d input data set
        // using the maximum number of work group items for this device
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
    glfwTerminate();
    
    return 0;
}
