#include "glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "Application.h"

void glfw_error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(glfw_error_callback);

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Animation Sample", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    GLuint gVertexArrayObject = 0;
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    Application* gApplication = new Application();
    gApplication->Initialize();

    glClearColor(0.4f, 0.3f, 0.4f, 0.0f);

    float lastTime = glfwGetTime();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float now = glfwGetTime();
        float dt = now - lastTime;

        int clientWidth, clientHeight;
		glfwGetWindowSize(window, &clientWidth, &clientHeight);

        if (gApplication != 0) {
            gApplication->Update(dt);
        }
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(gVertexArrayObject);

        float aspect = (float)clientWidth / (float)clientHeight;
        if (gApplication != 0) {
            gApplication->Render(aspect);
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        lastTime = now;
    }

    delete gApplication;
    gApplication = 0;

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &gVertexArrayObject);
    gVertexArrayObject = 0;
    
    glfwTerminate();
    return 0;
}
