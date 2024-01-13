

#ifndef PIPE_PIPE_H
#define PIPE_PIPE_H

#include "back.h"
#include "model.h"

void initialize(glm::vec3 camera, float FOV, int width, int height, float near = 0.1f, float far = 500.0f);

void update();

class Camera {
public:
    glm::vec3 position;

    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    glm::vec3 rotation;

    void rotateByMouse(double dx, double dy);
    glm::mat4 getView();

    Camera(glm::vec3 position, glm::vec3 rotation);
    Camera() {position = glm::vec3(0.0); rotation = glm::vec3(0.0);};

private:
    void getRotation();
};

class World {
public:
    static Camera camera;
};

#endif //PIPE_PIPE_H
