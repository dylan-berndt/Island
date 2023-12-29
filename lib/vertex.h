

#ifndef PIPE_VERTEX_H
#define PIPE_VERTEX_H

#include "back.h"

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

#endif //PIPE_VERTEX_H
