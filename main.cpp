
#include "lib/pipe.h"

using namespace std;

#define WIDTH (1920 * 3 / 4)
#define HEIGHT (1080 * 3 / 4)

#define SHADOW_WIDTH (2048)
#define SHADOW_HEIGHT (2048)

int bloomKernelSize = 4;

int screen_width = WIDTH;
int screen_height = HEIGHT;

float delta = 0;
double last_time = 0;

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
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    World::camera = Camera(glm::vec3(0.0, 4.0, 20.0), glm::vec3(0.0));
    ShaderProgram::perspective = glm::perspective(glm::radians(60.0f), float(WIDTH) / float(HEIGHT), 0.1f, 500.0f);

    glm::mat4 lightProjection = glm::ortho(-40.0f, 40.0f, -20.0f, 20.0f, 0.1f, 80.0f);
    glm::mat4 lightView = glm::lookAt(glm::vec3(-15.0f, 15.0f, -15.0f),
                                      glm::vec3( 0.0f, 0.0f,  0.0f),
                                      glm::vec3( 0.0f, 1.0f,  0.0f));
    ShaderProgram::lightSpace = lightProjection * lightView;

    Square screen;

    int amount = 800;
    Mesh water = flatMesh(amount, amount);
    water.model = glm::translate(water.model, glm::vec3(0.0, -1.5, 0.0));
    water.model = glm::scale(water.model, glm::vec3(400.0, 1.0, 400.0));

    ShaderProgram water_shader;
    water_shader.openShader("../Island/Shaders/water.vert", GL_VERTEX_SHADER);
    water_shader.openShader("../Island/Shaders/water.frag", GL_FRAGMENT_SHADER);
    water_shader.compile();

    ShaderProgram post_shader;
    post_shader.openShader("../Island/Shaders/post.vert", GL_VERTEX_SHADER);
    post_shader.openShader("../Island/Shaders/post.frag", GL_FRAGMENT_SHADER);
    post_shader.compile();

    ShaderProgram default_shader;
    default_shader.openShader("../Island/Shaders/default.vert", GL_VERTEX_SHADER);
    default_shader.openShader("../Island/Shaders/default.frag", GL_FRAGMENT_SHADER);
    default_shader.compile();

    ShaderProgram skybox_shader;
    skybox_shader.openShader("../Island/Shaders/skybox.vert", GL_VERTEX_SHADER);
    skybox_shader.openShader("../Island/Shaders/skybox.frag", GL_FRAGMENT_SHADER);
    skybox_shader.compile();

    ShaderProgram shadow_shader;
    shadow_shader.openShader("../Island/Shaders/shadow.vert", GL_VERTEX_SHADER);
    shadow_shader.openShader("../Island/Shaders/shadow.frag", GL_FRAGMENT_SHADER);
    shadow_shader.compile();

    vector<string> faces = {"../Island/Sky/bluecloud_rt.jpg", "../Island/Sky/bluecloud_lf.jpg",
                            "../Island/Sky/bluecloud_up.jpg", "../Island/Sky/bluecloud_dn.jpg",
                            "../Island/Sky/bluecloud_bk.jpg", "../Island/Sky/bluecloud_ft.jpg"};

    CubeMap skyMap(faces);
    SkyBox sky(skyMap);
    sky.model = glm::rotate(glm::mat4(1.0), glm::radians(-135.0f), glm::vec3(0.0, 1.0, 0.0));

    Model island("../Island/Models/Island/island.obj");
    Model crab("../Island/Models/Crab/10012_crab_v2_iterations-1.obj");
    Model palm("../Island/Models/Palm Plant/Palm_01.obj");
    Model tree("../Island/Models/Palm Tree/CoconutPalm.obj");

    crab.transform(glm::vec3(0.0, 2.0, 0.0), glm::vec3(-90.0, 0.0, 0.0), glm::vec3(0.125));
    palm.transform(glm::vec3(-4.0, 1.8, 4.0), glm::vec3(0.0), glm::vec3(0.25));
    tree.transform(glm::vec3(2.0, 0.4, -4.0), glm::vec3(-90.0, 0.0, 0.0), glm::vec3(0.125));

    Model rock_10("../Island/Models/Rocks/10/moss rock 10.obj");
    Model rock_12("../Island/Models/Rocks/12/moss rock 12.obj");
    Model rock_13("../Island/Models/Rocks/13/moss rock 13.obj");
    Model rock_14("../Island/Models/Rocks/14/moss rock 14.obj");

    rock_10.transform(Transform(glm::vec3(10.0, 2.0, -10.0)));
    rock_12.transform(Transform(glm::vec3(-10.0, 2.0, -10.0)));
    rock_13.transform(Transform(glm::vec3(-10.0, 2.0, 10.0)));
    rock_14.transform(Transform(glm::vec3(10.0, 2.0, 10.0)));

    FrameBuffer after_effects_buffer;

    Texture2D color_texture(WIDTH, HEIGHT, GL_RGB);
    Texture2D depth_texture(WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT);

    after_effects_buffer.attachTexture2D(GL_COLOR_ATTACHMENT0, color_texture);
    after_effects_buffer.attachTexture2D(GL_DEPTH_ATTACHMENT, depth_texture);

    FrameBuffer shadow_buffer;

    Texture2D shadow_texture(SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT);
    shadow_texture.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    shadow_texture.unbind();

    shadow_buffer.attachTexture2D(GL_DEPTH_ATTACHMENT, shadow_texture);
    shadow_buffer.bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    shadow_buffer.unbind();

    while (!glfwWindowShouldClose(win)) {
        delta = glfwGetTime() - last_time;
        cout << "Approx. FPS: " << 1.0 / delta << endl;
        last_time = glfwGetTime();

        update();

        water.model[3][0] = float(int(World::camera.position.x));
        water.model[3][2] = float(int(World::camera.position.z));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadow_buffer.bind();

        glClear(GL_DEPTH_BUFFER_BIT);

        shadow_shader.use();

        glDisable(GL_CULL_FACE);
        island.draw(shadow_shader);
        crab.draw(shadow_shader);
        palm.draw(shadow_shader);
        tree.draw(shadow_shader);

        rock_10.draw(shadow_shader);
        rock_12.draw(shadow_shader);
        rock_13.draw(shadow_shader);
        rock_14.draw(shadow_shader);

        glEnable(GL_CULL_FACE);

        shadow_shader.stop();

        shadow_buffer.unbind();

        glViewport(0, 0, WIDTH, HEIGHT);
        after_effects_buffer.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sky.draw(skybox_shader);

        water_shader.use();

        skyMap.bind();

        water.draw(water_shader);

        water_shader.stop();

        default_shader.use();

        shadow_texture.activate(GL_TEXTURE2);
        default_shader.setInt("shadowMap", 2);

        island.draw(default_shader);
        crab.draw(default_shader);
        palm.draw(default_shader);

        rock_10.draw(default_shader);
        rock_12.draw(default_shader);
        rock_13.draw(default_shader);
        rock_14.draw(default_shader);

        glDisable(GL_CULL_FACE);
        tree.draw(default_shader);
        glEnable(GL_CULL_FACE);

        default_shader.stop();

        after_effects_buffer.unbind();

        glViewport(0, 0, screen_width, screen_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        post_shader.use();

        depth_texture.activate(GL_TEXTURE0);
        color_texture.activate(GL_TEXTURE1);
        shadow_texture.activate(GL_TEXTURE2);

        post_shader.setInt("depth", 0);
        post_shader.setInt("color", 1);
        post_shader.setInt("shadowMap", 2);

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

bool focused = false;
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
