
#include "back.h"
#include "vertex.h"

VertexArray::VertexArray(float *v, int size, int mode) {
    vertices = v;
    amount = size / 3;
    unsigned int a, b; VAO = a; VBO = b;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(float), vertices, mode);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::bind() const {
    glBindVertexArray(VAO);
}

void VertexArray::draw() const {
    bind();
    glDrawArrays(GL_TRIANGLES, 0, amount);
    unbind();
}

IndexedArray::IndexedArray(float *v, int vs, unsigned int *i, int is, int mode) {
    vertices = v;
    indices = i;
    amount = is;
    unsigned int a, b, e; VAO = a; VBO = b; EBO = e;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vs * 3 * sizeof(float), vertices, mode);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, is * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void IndexedArray::unbind() {
    glBindVertexArray(0);
}

void IndexedArray::bind() const {
    glBindVertexArray(VAO);
}

void IndexedArray::draw() const {
    bind();
    glDrawElements(GL_TRIANGLES, amount, GL_UNSIGNED_INT, 0);
    unbind();
}
