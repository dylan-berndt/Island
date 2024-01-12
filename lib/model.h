

#ifndef PIPE_MODEL_H
#define PIPE_MODEL_H

#include "back.h"
#include "mesh.h"
#include "obj.h"

class Transform {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 matrix;

    void calcMatrix() {
        matrix = glm::translate(glm::mat4(1.0), position);
        matrix = glm::scale(matrix, scale);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
    };

    Transform(glm::vec3 p, glm::vec3 r, glm::vec3 s) {position = p; rotation = r; scale = s; calcMatrix();};
    Transform() {position = glm::vec3(0.0); rotation = glm::vec3(0.0); scale = glm::vec3(1.0); calcMatrix();};
    Transform(glm::vec3 p) {position = p; rotation = glm::vec3(0.0); scale = glm::vec3(1.0); calcMatrix();};
};

class Model {
public:
    Model(std::string path) {loadModel(path);}
    void draw(ShaderProgram &shader);

    void transform(Transform t) {
        model = t;
        for (int i = 0; i < meshes.size(); i++) {
            meshes[i].model = t.matrix;
        }
    };

    void transform(glm::vec3 p, glm::vec3 r, glm::vec3 s) {
        Transform t(p, r, s);
        transform(t);
    };

    std::vector<Mesh> meshes;

private:
    std::string directory;

    Transform model;

    void loadModel(std::string path);
    std::vector<Material> loadMaterials(std::string file);
};

#endif //PIPE_MODEL_H
