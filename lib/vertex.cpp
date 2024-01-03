
#include "back.h"
#include "vertex.h"

VertexArray::VertexArray(Vertex *v, int size, int mode) {
    vertices = v;
    amount = size;
    unsigned int a, b; VAO = a; VBO = b;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), vertices, mode);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void VertexArray::unbind() const {
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

IndexedArray::IndexedArray(Vertex *v, int vsize, unsigned int *i, int isize, int mode) : VertexArray(v, vsize, mode) {
    indices = i;
    amount = isize;

    unsigned int e; EBO = e;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

void IndexedArray::draw() const {
    bind();
    glDrawElements(GL_TRIANGLES, amount, GL_UNSIGNED_INT, 0);
    unbind();
}
