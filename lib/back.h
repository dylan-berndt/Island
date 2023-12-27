

#ifndef PIPE_BACK_H
#define PIPE_BACK_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>

class VertexArray {
public:
    float *vertices;
    void bind() const;
    void unbind();
    VertexArray(float *v, int size, int mode = GL_STATIC_DRAW);
private:
    unsigned int VAO;
    unsigned int VBO;
};

class ShaderProgram {
public:
    void openShader(const std::string& name, int type) const;
    void compile() const;
    void use() const;
    ShaderProgram();
private:
    unsigned int self;
};

float* flatMesh(float width, float depth, int nx, int nz, float height);
unsigned int genWater(float width, float depth, int nx, int nz, float height);

GLchar *shaderString(const std::string& name);

#endif //PIPE_BACK_H
