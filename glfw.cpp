#include "glfw.h"

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

void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
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
    
    *win = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "opencl raytracer - rhoeberg", NULL, NULL);
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

	glfwSetInputMode(*win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
