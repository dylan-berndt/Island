
#include "lib/back.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
    if (!glfwInit()) {
        cout << "Can't Initialize GLFW" << endl;
        exit (EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *win;
    win = glfwCreateWindow(800, 600, "Water", nullptr, nullptr);
    glfwMakeContextCurrent(win);

    gladLoadGL();

    glViewport(0, 0, 800,600);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

//    int nx = 100;
//    int nz = 100;
//    unsigned int water = genWater(100.0, 100.0, 100, 100, -1.0);

    float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
            0.0f,  0.5f, 0.0f  // top
    };

    VertexArray v(vertices, 3);
    v.bind();

    ShaderProgram red;
    red.openShader("../water.vert", GL_VERTEX_SHADER);
    red.openShader("../water.frag", GL_FRAGMENT_SHADER);
    red.compile();

    while (!glfwWindowShouldClose(win)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        red.use();
        v.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        v.unbind();

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << error << std::endl;
        }

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
