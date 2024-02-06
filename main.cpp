
#include "lib/pipe.h"

using namespace std;

int bloomKernelSize = 4;

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

void keyPress(GLFWwindow *window);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

float delta;

int main() {
    initialize();

    glfwSetKeyCallback(Window::self, keyCallback);
    glfwSetCursorPosCallback(Window::self, cursorPosCallback);

    ShaderProgram::width = Window::width;
    ShaderProgram::height = Window::height;

    ShaderProgram water_shader(File::getPath("Island/Shaders/water"));
    ShaderProgram post_shader(File::getPath("Island/Shaders/post"));

    ShaderProgram::postShader = &post_shader;

    vector<string> faces = {File::getPath("Island/Sky/bluecloud_rt.jpg"), File::getPath("Island/Sky/bluecloud_lf.jpg"),
                            File::getPath("Island/Sky/bluecloud_dn.jpg"), File::getPath("Island/Sky/bluecloud_up.jpg"),
                            File::getPath("Island/Sky/bluecloud_bk.jpg"), File::getPath("Island/Sky/bluecloud_ft.jpg")};

    CubeMap skyMap(faces);

    SkyBox sky(skyMap);
    Transform skyRotate(glm::vec3(0.0), glm::vec3(0.0, -135.0f, 0.0), glm::vec3(1.0));
    Entity skybox("skybox", &skyRotate);
    MeshComponent<SkyBox> skyMesh(sky, *ShaderProgram::skyboxShader);
    skybox.addComponent(&skyMesh);

    int amount = 400;
    Mesh waterMesh = flatMesh(amount, amount);
    Transform waterTransform(glm::vec3(0.0, -2.5, 0.0), glm::vec3(0.0), glm::vec3(400.0, 1.0, 400.0));
    Entity water("water", &waterTransform);
    MeshComponent<Mesh> mesh(waterMesh, water_shader);
    mesh.getMaterial().setCubeMap("sky", skyMap);
    water.addComponent(&mesh);

//    Transform textTransform(glm::vec3(-40.0, -40.0, 0.0), glm::vec3(0.0), glm::vec3(4.0));
//    Entity text("text", &textTransform);
//    Font arial("C:/Windows/Fonts/arial.ttf", 48);
//    TextComponent textComponent(&arial, "hey");
//    text.addComponent(&textComponent);

    cout << "Total vertices: " << Mesh::totalVertices << endl;

    while (!glfwWindowShouldClose(Window::self)) {
        update(delta);

        water.transform()->position.x = float(int(World::camera.position.x / 2.0f)) * 2.0f;
        water.transform()->position.z = float(int(World::camera.position.z / 2.0f)) * 2.0f;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setInt("bloomKernelSize", bloomKernelSize);
        ShaderProgram::postShader->stop();

        Window::draw();

        glfwPollEvents();
        keyPress(Window::self);

        glfwSwapBuffers(Window::self);
    }

    glfwDestroyWindow(Window::self);
    glfwTerminate();

    return 0;
}

bool focused = false;
double mx, my = 0;

bool orthographic = false;

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    if (focused && !(mx == 0 && my == 0)) {
        World::camera.rotateByMouse(xpos - mx, ypos - my);
    }
    mx = xpos;
    my = ypos;
}

float cameraSpeed = 10.0;
float speedMultiplier = 4.0;

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
        speed *= speedMultiplier;
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
            if (key == GLFW_KEY_P) {
                orthographic = !orthographic;
                World::camera.orthographic = orthographic;
            }
        }
        else {

        }
    }
}
