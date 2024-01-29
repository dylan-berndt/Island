

#ifndef PIPE_MODEL_H
#define PIPE_MODEL_H

//#include "imports.h"
#include "../imports.h"
#include "mesh.h"
#include "obj.h"

class Model {
public:
    Model(std::string path) {loadModel(path);}
    void draw(ShaderProgram &shader);

    std::vector<Mesh> meshes;

private:
    std::string directory;

    void loadModel(std::string path);
    std::vector<Material> loadMaterials(std::string file);
};

#endif //PIPE_MODEL_H
