

#ifndef PIPE_PIPE_H
#define PIPE_PIPE_H

#include "back.h"
#include "model.h"

class Camera {
public:
    glm::vec3 position;

    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    float pitch;
    float yaw;
    float roll;

    void rotateByMouse(double dx, double dy);
    glm::mat4 getView();

    Camera(glm::vec3 position, float pitch, float yaw, float roll);

private:
    void getRotation();
};

class World {
public:
    static Camera camera;
};

#endif //PIPE_PIPE_H
