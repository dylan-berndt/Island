

#ifndef PIPE_BACK_H
#define PIPE_BACK_H

#include "external/glad/glad.h"

float* flatMesh(float width, float depth, int dx, int dz, float height);
unsigned int bindVertices(float* vertices, int mode = GL_STATIC_DRAW);

#endif //PIPE_BACK_H
