
#include "pipe.h"

using namespace std;

void initialize() {

}

void update() {
    ShaderProgram::camera = World::camera.position;
    ShaderProgram::view = World::camera.getView();
}

Camera World::camera;

Camera::Camera(glm::vec3 pos, glm::vec3 rot) {
    position = pos;
    rotation = rot;

    getRotation();
}

void Camera::rotateByMouse(double dx, double dy) {
    rotation.y += dx * 0.1;
    rotation.x += dy * 0.1;

    rotation.x = glm::clamp(rotation.x, -79.0f, 79.0f);

    getRotation();
}

glm::mat4 Camera::getView() {
    glm::mat4 view(1.0);

    view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    view = glm::translate(view, -position);

    return view;
}

void Camera::getRotation() {
    glm::mat4 view(1.0);
    view = glm::rotate(view, glm::radians(-rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(-rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    forward = glm::vec3(view * glm::vec4(glm::vec3(0.0, 0.0, -1.0), 0.0));
    up = glm::vec3(view * glm::vec4(glm::vec3(0.0, 1.0, 0.0), 0.0));
    right = glm::vec3(view * glm::vec4(glm::vec3(1.0, 0.0, 0.0), 0.0));
}


