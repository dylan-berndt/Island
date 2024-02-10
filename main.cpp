
#include "lib/pipe.h"

using namespace std;

int bloomKernelSize = 4;

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

void keyPress(GLFWwindow *window);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void myCommand(string);

float delta;

int main() {
    Window::setUserCommand(myCommand);

    initialize();

    Window::setUserKeyCallback(keyCallback);
    Window::setUserKeyPress(keyPress);
    glfwSetCursorPosCallback(Window::self, cursorPosCallback);

    ShaderProgram::width = Window::width;
    ShaderProgram::height = Window::height;

    ShaderProgram post_shader(File::getPath("Island/Shaders/post"));

    ShaderProgram::postShader = &post_shader;

    while (!glfwWindowShouldClose(Window::self)) {
        update(delta);

        if (Entity::entityMap.count("water")) {
            Entity::entityMap["water"]->transform()->position.x = float(int(World::camera.position.x / 2.0f)) * 2.0f;
            Entity::entityMap["water"]->transform()->position.z = float(int(World::camera.position.z / 2.0f)) * 2.0f;
        }

        Window::draw();
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

void myCommand(string command) {
    istringstream iss(command);

    string type;
    iss >> type;

    if (type == "SATURATION") {
        float sat;
        iss >> sat;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setFloat("saturation", sat);
        ShaderProgram::postShader->stop();
    }
    else if (type == "HUE") {
        float hue;
        iss >> hue;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setFloat("hue", hue);
        ShaderProgram::postShader->stop();
    }
    else if (type == "VALUE") {
        float val;
        iss >> val;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setFloat("value", val);
        ShaderProgram::postShader->stop();
    }
    else if (type == "PIXEL") {
        int size;
        iss >> size;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setInt("pixelSize", size);
        ShaderProgram::postShader->stop();
    }
    else if (type == "OUTLINE") {
        float size;
        iss >> size;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setFloat("outlineThickness", size);
        ShaderProgram::postShader->stop();
    }
    else if (type == "BLOOM_KERNEL") {
        int size;
        iss >> size;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setInt("bloomKernelSize", size);
        ShaderProgram::postShader->stop();
    }
    else if (type == "BLOOM_EFFECT") {
        float effect;
        iss >> effect;

        ShaderProgram::postShader->use();
        ShaderProgram::postShader->setFloat("bloomEffect", effect);
        ShaderProgram::postShader->stop();
    }
    else if (type == "WATER") {
        ShaderProgram water_shader(File::getPath("Island/Shaders/water"));
        CubeMap skyMap(File::getPath("Island/Skybox/Day/"));

        int amount = 400;
        Mesh waterMesh = flatMesh(amount, amount);
        Transform *waterTransform = new Transform(glm::vec3(0.0, -2.5, 0.0), glm::vec3(0.0), glm::vec3(400.0, 1.0, 400.0));
        Entity *water = new Entity("water", waterTransform);
        MeshComponent<Mesh> *mesh = new MeshComponent<Mesh>(waterMesh, water_shader);
        mesh->getMaterial().setCubeMap("sky", skyMap);
        water->addComponent("MeshComponent", mesh);
    }
    else if (type == "WATER_SPECULAR") {
        float effect;
        iss >> effect;

        Component *waterComponent = Entity::entityMap["water"]->getComponent("MeshComponent");
        MeshComponent<Mesh> *waterMesh = dynamic_cast<MeshComponent<Mesh> *>(waterComponent);
        ShaderProgram waterShader = waterMesh->shader;

        waterShader.use();
        waterShader.setFloat("specularStrength", effect);
        waterShader.stop();
    }
    else if (type == "STATS") {
        Log << endl << "Total vertices: " << Mesh::totalVertices << endl;
        Log << endl << "Entities: " << endl;
        for (auto entity : Entity::entities) {
            Log << entity->name << endl;
        }
    }
}
