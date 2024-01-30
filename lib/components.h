
#ifndef ISLAND_COMPONENTS_H
#define ISLAND_COMPONENTS_H

#include "imports.h"
#include "game.h"
#include "model.h"
#include "obj.h"

class Camera : public Component {
public:
    glm::vec3 position;

    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    glm::vec3 rotation;

    float nearPlane = 0.1f;
    float farPlane = 500.0f;
    float fov = 60.0f;
    float aspect = 1600.0 / 900.0;

    float size = 20.0f;
    bool orthographic = false;

    void rotateByMouse(double dx, double dy);
    glm::mat4 getView();
    glm::mat4 getProjection();

    void update(float delta) override {};
    void draw() override {};
    void draw(ShaderProgram &) override {};

    Camera(glm::vec3 position, glm::vec3 rotation);
    Camera() {position = glm::vec3(0.0); rotation = glm::vec3(0.0);};

private:
    void getRotation();
};

template <typename M = Mesh>
class MeshComponent : public Component {
public:
    void update(float delta) {
        Entity* ptr = Entity::get(entity);
        mesh->model = ptr->transform()->matrix;
    };
    void draw() {
        shader.use();
        mesh->draw(shader);
        shader.stop();
    };
    void draw(ShaderProgram &s) override {
        mesh->draw(s);
    }

    M* mesh;
    ShaderProgram shader;

    Material getMaterial() {return mesh->material;};

    MeshComponent(std::vector<Vertex> vertices, std::vector<int> indices, ShaderProgram &s = *ShaderProgram::defaultShader) : mesh(vertices, indices) {
        shader = s;
    };

    MeshComponent(const M& m, ShaderProgram &s = *ShaderProgram::defaultShader) : mesh(new M(m)){
        shader = s;
    };
};

class ModelComponent : public Component {
public:
    void update(float delta) override;
    void draw() override;
    void draw(ShaderProgram &) override;

    Model model;
    ShaderProgram shader;

    explicit ModelComponent(std::string path, ShaderProgram &s = *ShaderProgram::defaultShader);
    explicit ModelComponent(Model m, ShaderProgram &s = *ShaderProgram::defaultShader);
};

#endif //ISLAND_COMPONENTS_H
