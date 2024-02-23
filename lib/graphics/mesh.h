
#ifndef PIPE_MESH_H
#define PIPE_MESH_H

#include "../imports.h"
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
    glm::vec3 tangent = glm::vec3(0.0, 0.0, 0.0);
};

class Material {
public:
    float roughness = 0.0f;
    float glossiness = 0.0f;
    float metallic = 0.0f;
    float opacity = 0.0f;
    float transparency = 0.0f;

    Texture2D opacityTexture;
    Texture2D emissiveTexture;
    Texture2D ambientOcclusionTexture;
    Texture2D metallicTexture;
    Texture2D glossinessTexture;
    Texture2D roughnessTexture;
    Texture2D specularTexture;

    std::string name;

    int assign(std::string n, bool value) {booleans.insert(std::pair<std::string, bool>(n, value)); return 1;};
    int assign(std::string n, int value) {integers.insert(std::pair<std::string, int>(n, value)); return 1;};
    int assign(std::string n, float value) {floats.insert(std::pair<std::string, float>(n, value)); return 1;};
    int assign(std::string n, std::string value) {return 0;};
    int assign(std::string n, glm::vec4 value) {vec4s.insert(std::pair<std::string, glm::vec3>(n, value)); return 1;};
    int assign(std::string n, glm::vec3 value) {vec3s.insert(std::pair<std::string, glm::vec3>(n, value)); return 1;};
    int assign(std::string n, glm::vec2 value) {vec2s.insert(std::pair<std::string, glm::vec2>(n, value)); return 1;};
    int assign(std::string n, glm::mat4 value) {mat4s.insert(std::pair<std::string, glm::mat4>(n, value)); return 1;};
    int assign(std::string n, Texture2D value) {textures.insert(std::pair<std::string, Texture2D>(n, value)); return 1;};
    int assign(std::string n, CubeMap value) {cubeMaps.insert(std::pair<std::string, CubeMap>(name, value)); return 1;};
    int assign(std::string n, Texture2DArray value) {textureArrays.insert(std::pair<std::string, Texture2DArray>(n, value)); return 1;};

    void use(ShaderProgram &shader);

    Material() = default;

private:
    std::map<std::string, bool> booleans;
    std::map<std::string, int> integers;
    std::map<std::string, float> floats;
    std::map<std::string, glm::vec3> vec4s;
    std::map<std::string, glm::vec3> vec3s;
    std::map<std::string, glm::vec2> vec2s;
    std::map<std::string, Texture2D> textures;
    std::map<std::string, glm::mat4> mat4s;
    std::map<std::string, CubeMap> cubeMaps;
    std::map<std::string, Texture2DArray> textureArrays;
};

class Mesh {
public:
    static int totalVertices;

    std::vector<Vertex> vertices;
    std::vector<int> indices;

    glm::mat4 model;

    Material material;

    void bind() {glBindVertexArray(VAO);};

    void resetMesh(std::vector<Vertex>, std::vector<int>);

    void calculateTangents();

    Mesh(std::vector<Vertex>, std::vector<int>);
    void draw(ShaderProgram&);

protected:
    unsigned int VAO, VBO, EBO;
    Mesh() {};

private:
    void setupMesh();
};

Mesh flatMesh(int, int);

class DynamicMesh : public Mesh {
public:
    DynamicMesh(std::vector<Vertex>, std::vector<int>);
    void updateSubData(std::vector<Vertex>, int offset = 0);
    void updateSubData(std::vector<int>, int offset = 0);
private:
    void setupDynamicMesh();
};

#endif //PIPE_MESH_H
