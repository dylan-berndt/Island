
#include "pipe.h"
using namespace std;

Camera::Camera(glm::vec3 pos, float p, float y, float r) {
    position = pos;
    pitch = p;
    yaw = y;
    roll = r;

    getRotation();
}

void Camera::rotateByMouse(double dx, double dy) {
    yaw += dx * 0.1;
    pitch += dy * 0.1;

    pitch = glm::clamp(pitch, -79.0f, 79.0f);

    getRotation();
}

glm::mat4 Camera::getView() {
    glm::mat4 view(1.0);

    view = glm::rotate(view, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));

    view = glm::translate(view, -position);

    return view;
}

void Camera::getRotation() {
    glm::mat4 view(1.0);
    view = glm::rotate(view, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));

    forward = glm::vec3(view * glm::vec4(glm::vec3(0.0, 0.0, -1.0), 0.0));
    up = glm::vec3(view * glm::vec4(glm::vec3(0.0, 1.0, 0.0), 0.0));
    right = glm::vec3(view * glm::vec4(glm::vec3(1.0, 0.0, 0.0), 0.0));
}


