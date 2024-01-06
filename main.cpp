
#include "lib/pipe.h"
using namespace std;

#define WIDTH (1920 / 2)
#define HEIGHT (1080 / 2)

int bloomKernelSize = 4;

int screen_width = WIDTH;
int screen_height = HEIGHT;

float delta = 0;
double last_time = 0;

glm::mat4 ShaderProgram::perspective = glm::perspective(glm::radians(70.0f), float(WIDTH) / float(HEIGHT), 0.1f, 500.0f);
glm::mat4 ShaderProgram::view;
glm::vec3 ShaderProgram::camera;

Camera World::camera(glm::vec3(0.0, 4.0, 20.0), 0, 0, 0);

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

void keyPress(GLFWwindow *window);
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
    glfwSetCursorPosCallback(win, cursorPosCallback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    int amount = 200;
    Mesh water = flatMesh(amount, amount);
    water.model = glm::translate(water.model, glm::vec3(0.0, 0.0, 0.0));
    water.model = glm::scale(water.model, glm::vec3(200.0, 1.0, 200.0));

    vector<Vertex> vertices = {
            Vertex(glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0)),
            Vertex(glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0)),
            Vertex(glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0)),
            Vertex(glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0))
    };

    vector<int> indices = {0, 1, 2, 1, 3, 2};

    Mesh screen(vertices, indices, vector<Texture2D>());

    ShaderProgram water_shader;
    water_shader.openShader("../water.vert", GL_VERTEX_SHADER);
    water_shader.openShader("../water.frag", GL_FRAGMENT_SHADER);
    water_shader.compile();

    ShaderProgram post_shader;
    post_shader.openShader("../post.vert", GL_VERTEX_SHADER);
    post_shader.openShader("../post.frag", GL_FRAGMENT_SHADER);
    post_shader.compile();

    ShaderProgram default_shader;
    default_shader.openShader("../default.vert", GL_VERTEX_SHADER);
    default_shader.openShader("../default.frag", GL_FRAGMENT_SHADER);
    default_shader.compile();

    Model island("../Island/island.obj");
    Model crab("../Island/10012_crab_v2_iterations-1.obj");

    island.meshes[0].model = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 2.0, 0.0));

    glm::mat4 crabModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 4.0, 0.0));
    crabModel = glm::scale(crabModel, glm::vec3(0.125));
    crabModel = glm::rotate(crabModel, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    crab.meshes[0].model = crabModel;

    FrameBuffer buffer;

    Texture2D color_texture("", WIDTH, HEIGHT, GL_RGB);
    Texture2D depth_texture("", WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT);

    buffer.attachTexture2D(GL_COLOR_ATTACHMENT0, color_texture);
    buffer.attachTexture2D(GL_DEPTH_ATTACHMENT, depth_texture);

    while (!glfwWindowShouldClose(win)) {
        delta = glfwGetTime() - last_time;
//        cout << 1.0 / delta << endl;
        last_time = glfwGetTime();

        ShaderProgram::camera = World::camera.position;
        ShaderProgram::view = World::camera.getView();

        glViewport(0, 0, WIDTH, HEIGHT);
        buffer.bind();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        water_shader.use();

        water.draw(water_shader);

        water_shader.stop();

        default_shader.use();

        island.draw(default_shader);
        crab.draw(default_shader);

        default_shader.stop();

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

        screen.draw(post_shader);

        post_shader.stop();

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            cerr << "OpenGL error: " << err << endl;
        }

        glfwPollEvents();
        keyPress(win);

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}

bool focused = true;
double mx, my = 0;

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    if (focused && !(mx == 0 && my == 0)) {
        World::camera.rotateByMouse(xpos - mx, ypos - my);
    }
    mx = xpos;
    my = ypos;
}

float cameraSpeed = 10.0;

void keyPress(GLFWwindow *window) {
    glm::vec3 groundAlignedForward = World::camera.forward;
    groundAlignedForward.y = 0.0;
    groundAlignedForward = glm::normalize(groundAlignedForward);

    glm::vec3 movement(0.0);

    int state = glfwGetKey(window, GLFW_KEY_W);
    if (state == GLFW_PRESS) {
        movement += groundAlignedForward;
    }

    state = glfwGetKey(window, GLFW_KEY_S);
    if (state == GLFW_PRESS) {
        movement -= groundAlignedForward;
    }

    state = glfwGetKey(window, GLFW_KEY_D);
    if (state == GLFW_PRESS) {
        movement += World::camera.right;
    }

    state = glfwGetKey(window, GLFW_KEY_A);
    if (state == GLFW_PRESS) {
        movement -= World::camera.right;
    }

    state = glfwGetKey(window, GLFW_KEY_SPACE);
    if (state == GLFW_PRESS) {
        movement += glm::vec3(0.0, 1.0, 0.0);
    }

    state = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    if (state == GLFW_PRESS) {
        movement -= glm::vec3(0.0, 1.0, 0.0);
    }

    float speed = cameraSpeed;
    state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    if (state == GLFW_PRESS) {
        speed *= 2.0;
    }

    if (movement.x != 0.0 || movement.y != 0.0 || movement.z != 0.0) {
        World::camera.position += glm::normalize(movement) * speed * delta;
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            if (focused) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                focused = false;
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                focused = true;
            }
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
        else {

        }
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    screen_width = width;
    screen_height = height;
}
