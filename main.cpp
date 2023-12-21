#include <iostream>
#include "lib/back.h"
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
    if (!glfwInit()) {
        std::cout << "Can't Initialize GLFW" << std::endl;
        exit (EXIT_FAILURE);
    }

    GLFWwindow *win;
    win = glfwCreateWindow (800, 600, "Water", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
    glfwMakeContextCurrent(win);
    glViewport(0, 0, 800,600);

    //float* mesh = flatMesh(4.0, 4.0, 100, 100, -2.0);

    while (!glfwWindowShouldClose(win)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwWaitEvents();
        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
