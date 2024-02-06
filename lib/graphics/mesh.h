
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

    std::string name = "";

    void setBool(std::string name, bool value) {booleans.insert(std::pair<std::string, bool>(name, value));};
    void setInt(std::string name, int value) {integers.insert(std::pair<std::string, int>(name, value));};
    void setFloat(std::string name, float value) {floats.insert(std::pair<std::string, float>(name, value));};
    void setVec3(std::string name, glm::vec3 value) {vec3s.insert(std::pair<std::string, glm::vec3>(name, value));};
    void setVec2(std::string name, glm::vec2 value) {vec2s.insert(std::pair<std::string, glm::vec2>(name, value));};
    void setMat4(std::string name, glm::mat4 value) {mat4s.insert(std::pair<std::string, glm::mat4>(name, value));};
    void setTexture2D(std::string name, Texture2D value) {textures.insert(std::pair<std::string, Texture2D>(name, value));};
    void setCubeMap(std::string name, CubeMap value) {cubeMaps.insert(std::pair<std::string, CubeMap>(name, value));};
    void setTexture2DArray(std::string name, Texture2DArray value) {textureArrays.insert(std::pair<std::string, Texture2DArray>(name, value));};

    void use(ShaderProgram &shader);

    Material() = default;

private:
    std::map<std::string, bool> booleans;
    std::map<std::string, int> integers;
    std::map<std::string, float> floats;
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
