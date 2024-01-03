
#ifndef PIPE_MESH_H
#define PIPE_MESH_H

#include "back.h"
#include "vertex.h"
#include "textures.h"
#include "shader.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::vector<Texture2D> textures;

    glm::mat4 model;

    Mesh(std::vector<Vertex>, std::vector<int>, std::vector<Texture2D>);
    void draw(ShaderProgram&);
    void draw();
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

Mesh flatMesh(int, int);

#endif //PIPE_MESH_H
