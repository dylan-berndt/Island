

#ifndef PIPE_PIPE_H
#define PIPE_PIPE_H

#include "back.h"
#include "vertex.h"
#include "shader.h"
#include "textures.h"

float* flatMesh(float width, float depth, int nx, int nz, float height);
IndexedArray genWater(float width, float depth, int nx, int nz, float height);

GLchar *shaderString(const std::string& name);

#endif //PIPE_PIPE_H
