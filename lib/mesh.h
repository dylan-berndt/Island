
#ifndef PIPE_MESH_H
#define PIPE_MESH_H

#include "back.h"
#include "textures.h"
#include "shader.h"

class Vertex {
public:
    Vertex() {
        position = glm::vec3(0.0); normal = glm::vec3(0.0); texCoord = glm::vec2(0.0);
    }
    Vertex(glm::vec3 p, glm::vec3 c = glm::vec3(0.0), glm::vec2 t = glm::vec2(0.0)) {
        position = p; normal = c; texCoord = t;
    }
    Vertex(glm::vec3 p, glm::vec2 t) {
        position = p; normal = glm::vec3(0.0); texCoord = t;
    }
    Vertex(float x, float y, float z) {
        position = glm::vec3(x, y, z); normal = glm::vec3(0.0); texCoord = glm::vec2(0.0);
    }
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 color = glm::vec3(0, 0, 0);
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    std::vector<Texture2D> textures;

    glm::mat4 model;

    Mesh(std::vector<Vertex>, std::vector<int>, std::vector<Texture2D>);
    void draw(ShaderProgram&);
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

Mesh flatMesh(int, int);

#endif //PIPE_MESH_H
