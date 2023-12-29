

#ifndef PIPE_BACK_H
#define PIPE_BACK_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


class VertexArray {
public:
    void bind() const;
    static void unbind();
    void draw() const;
    VertexArray(float *v, int size, int mode = GL_STATIC_DRAW);
private:
    unsigned int VAO;
    unsigned int VBO;
    float *vertices;
    int amount;
};

class IndexedArray {
public:
    void bind() const;
    static void unbind();
    void draw() const;
    IndexedArray(float *v, int vsize, unsigned int *i, int isize, int mode = GL_STATIC_DRAW);
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    float *vertices;
    unsigned int *indices;
    int amount;
};

class ShaderProgram {
public:
    void openShader(const std::string& name, int type) const;
    void compile() const;
    void use() const;
    void stop() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;

    ShaderProgram();
private:
    unsigned int self;
};

float* flatMesh(float width, float depth, int nx, int nz, float height);
IndexedArray genWater(float width, float depth, int nx, int nz, float height);

GLchar *shaderString(const std::string& name);

#endif //PIPE_BACK_H
