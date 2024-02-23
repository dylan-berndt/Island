
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

    Entity *camera = Entity::entityMap["Camera"];
    Transform *cameraTransform = camera->transform();

    Entity light("light");
    light.addComponent("LightComponent", new LightComponent(glm::vec3(1.0)));

    while (!glfwWindowShouldClose(Window::self)) {
        update(delta);



        if (Entity::entityMap.count("water")) {
            Entity::entityMap["water"]->transform()->position.x = float(int(cameraTransform->position.x / 2.0f)) * 2.0f;
            Entity::entityMap["water"]->transform()->position.z = float(int(cameraTransform->position.z / 2.0f)) * 2.0f;
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

    Entity *camera = Entity::entityMap["Camera"];
    Transform *cameraTransform = camera->transform();

    if (movement.x != 0.0 || movement.y != 0.0 || movement.z != 0.0) {
        cameraTransform->position += glm::normalize(movement) * speed * delta;
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

class Water : public Component {
public:
    void update(float d) override {
        Entity *camera = Entity::entityMap["Camera"];
        Transform *cameraTransform = camera->transform();

        Entity* ptr = Entity::get(entity);
        ptr->transform()->position.x = float(int(cameraTransform->position.x / 2.0f)) * 2.0f;
        ptr->transform()->position.z = float(int(cameraTransform->position.x / 2.0f)) * 2.0f;
    };
    void draw() override {};
    void draw(ShaderProgram &) override {};
};

void myCommand(string command) {
    istringstream iss(command);

    string type;
    iss >> type;

    if (type == "WATER") {
        ShaderProgram water_shader(File::getPath("Island/Shaders/water"));
        CubeMap skyMap(File::getPath("Island/Skybox/Day/"));

        int amount;
        iss >> amount;

        double scale;
        iss >> scale;

        double y;
        iss >> y;

        Mesh waterMesh = flatMesh(amount, amount);
        Transform *waterTransform = new Transform(glm::vec3(0.0, -2.5, 0.0), glm::vec3(0.0), glm::vec3(scale, 1.0, scale));
        Entity *water = new Entity("water" + to_string(amount) + to_string(scale), waterTransform);
        MeshComponent<Mesh> *mesh = new MeshComponent<Mesh>(waterMesh, water_shader);
        mesh->getMaterial().assign("sky", skyMap);
        water->addComponent("MeshComponent", mesh);
        water->addComponent("WaterComponent", new Water());
    }
    else if (type == "STATS") {
        Log << endl << "Total vertices: " << Mesh::totalVertices << endl;
        Log << endl << "Entities: " << endl;
        for (auto entity : Entity::entities) {
            Log << entity->name << endl;
        }
    }
}
