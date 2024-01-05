

#ifndef PIPE_MODEL_H
#define PIPE_MODEL_H

#include "back.h"
#include "mesh.h"
#include "obj.h"

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
