

#ifndef PIPE_MODEL_H
#define PIPE_MODEL_H

#include "back.h"
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
    Material loadMaterial(std::string file, std::string &name);
};

#endif //PIPE_MODEL_H
