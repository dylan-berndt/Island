

#ifndef PIPE_MODEL_H
#define PIPE_MODEL_H

#include "back.h"
#include "mesh.h"
#include "obj.h"

class Material {
public:
    glm::vec3 baseColor;
    glm::vec3 specularColor;
    glm::vec3 emissiveColor;

    float roughness;
    float glossiness;
    float metallic;
    float opacity;
    float transparency;

    Texture2D opacityTexture;
    Texture2D normalTexture;
    Texture2D emissiveTexture;
    Texture2D ambientOcclusionTexture;
    Texture2D metallicTexture;
    Texture2D glossinessTexture;
    Texture2D roughnessTexture;
    Texture2D baseTexture;
    Texture2D specularTexture;

    Material() {};
};

class Model {
public:
    Model(std::string path) {loadModel(path);}
    void draw(ShaderProgram &shader);
private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    Material loadMaterial(std::string path);
    std::vector<Texture2D> loadMaterialTextures(Material *mat, std::string typeName);
};

#endif //PIPE_MODEL_H
