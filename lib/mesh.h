
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
    Vertex(glm::vec3 p, glm::vec3 n = glm::vec3(0.0), glm::vec2 t = glm::vec2(0.0)) {
        position = p; normal = n; texCoord = t;
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

class Material {
public:
    glm::vec3 baseColor = glm::vec3(1.0, 0.0, 1.0);
    glm::vec3 specularColor = glm::vec3(1.0, 0.0, 1.0);
    glm::vec3 emissiveColor = glm::vec3(1.0, 0.0, 1.0);

    float roughness = 0.0f;
    float glossiness = 0.0f;
    float metallic = 0.0f;
    float opacity = 0.0f;
    float transparency = 0.0f;

    Texture2D opacityTexture;
    Texture2D normalTexture;
    Texture2D emissiveTexture;
    Texture2D ambientOcclusionTexture;
    Texture2D metallicTexture;
    Texture2D glossinessTexture;
    Texture2D roughnessTexture;
    Texture2D baseTexture;
    Texture2D specularTexture;

    std::string name = "";

    Material() {};
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<int> indices;

    glm::mat4 model;

    Material material;

    void bind() {glBindVertexArray(VAO);};

    void resetMesh(std::vector<Vertex>, std::vector<int>);

    Mesh(std::vector<Vertex>, std::vector<int>);
    void draw(ShaderProgram&);
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

Mesh flatMesh(int, int);


#endif //PIPE_MESH_H
