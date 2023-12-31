
#include "lib/pipe.h"
using namespace std;

#define WIDTH (1920 / 2)
#define HEIGHT (1080 / 2)

int bloomKernelSize = 4;

int screen_width = WIDTH;
int screen_height = HEIGHT;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
    if (!glfwInit()) {
        cout << "Can't Initialize GLFW" << endl;
        exit (EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *win;
    win = glfwCreateWindow(WIDTH, HEIGHT, "Water Demo", nullptr, nullptr);
    glfwMakeContextCurrent(win);

    gladLoadGL();

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(win, framebufferSizeCallback);
    glfwSetKeyCallback(win, keyCallback);

    glEnable(GL_DEPTH_TEST);

    int amount = 100;
    IndexedArray water = genWater(50.0, 50.0, amount, amount, -1.0);

    Vertex vertices[] = {
            Vertex(glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0)),
            Vertex(glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0)),
            Vertex(glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0)),
            Vertex(glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0)),
            Vertex(glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0)),
            Vertex(glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0))
    };
    VertexArray screen(vertices, 6);

    ShaderProgram water_shader;
    water_shader.openShader("../water.vert", GL_VERTEX_SHADER);
    water_shader.openShader("../water.frag", GL_FRAGMENT_SHADER);
    water_shader.compile();

    ShaderProgram post_shader;
    post_shader.openShader("../post.vert", GL_VERTEX_SHADER);
    post_shader.openShader("../post.frag", GL_FRAGMENT_SHADER);
    post_shader.compile();

    FrameBuffer buffer;

    Texture2D color_texture("", WIDTH, HEIGHT, GL_RGB);
    Texture2D depth_texture("", WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT);

    buffer.attachTexture2D(GL_COLOR_ATTACHMENT0, color_texture);
    buffer.attachTexture2D(GL_DEPTH_ATTACHMENT, depth_texture);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);

    while (!glfwWindowShouldClose(win)) {
        glViewport(0, 0, WIDTH, HEIGHT);
        buffer.bind();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        water_shader.use();

        glm::mat4 identity(1.0);

        glm::mat4 view = glm::rotate(identity, glm::radians(15.0f), glm::vec3(1.0, 0.0, 0.0));
        view = glm::rotate(view, float(glm::radians(glfwGetTime() * 10.0f)), glm::vec3(0.0, 1.0, 0.0));

        water_shader.setMat4("projection", projection);
        water_shader.setMat4("view", view);
        water_shader.setMat4("model", glm::translate(identity, glm::vec3(0.0, -2.0, 0.0)));
        water_shader.setVec3("camera", glm::vec3(0.0, 2.0, 0.0));

        water_shader.setFloat("time", float(glfwGetTime()));

        water.draw();

        water_shader.stop();

        buffer.unbind();

        glViewport(0, 0, screen_width, screen_height);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        post_shader.use();

        depth_texture.activate(GL_TEXTURE0);
        color_texture.activate(GL_TEXTURE1);

        post_shader.setInt("depth", 0);
        post_shader.setInt("color", 1);

        post_shader.setInt("kernelSize", bloomKernelSize);

        screen.draw();

        post_shader.stop();

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            cerr << "OpenGL error: " << err << endl;
        }

        glfwPollEvents();
        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
        if (mods & GLFW_MOD_CONTROL) {
            if (key == GLFW_KEY_EQUAL) {
                bloomKernelSize *= 2;
                bloomKernelSize = bloomKernelSize > 16 ? 16 : bloomKernelSize;
            }
            if (key == GLFW_KEY_MINUS) {
                if (bloomKernelSize > 1) {
                    bloomKernelSize /= 2;
                }
                else {
                    bloomKernelSize = 1;
                }
            }
        }
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    screen_width = width;
    screen_height = height;
}
