
#ifndef PIPE_COMPONENTS_H
#define PIPE_COMPONENTS_H

#include "imports.h"
#include "game.h"
#include "model.h"
#include "obj.h"
#include "text.h"

class Camera : public Component {
public:
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

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

    Camera() {};

    template <typename T>
    int assign(std::string name, T value) {
        if (name == "fov") { fov = value;}
        else if (name == "aspect") {aspect = value;}
        else if (name == "size") {size = value;}
        else { return 0;}
        return 1;
    }

    int assign(std::string name, bool value) {
        if (name == "orthographic") { orthographic = value;}
        else { return 0;}
        return 1;
    }

private:
    void getRotation();
};

template <typename M = Mesh>
class MeshComponent : public Component {
public:
    void update(float delta) override {
        Entity* ptr = Entity::get(entity);
        mesh->model = ptr->transform()->matrix;
    };
    void draw() override {
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

    bool cullFace = true;

    explicit ModelComponent(std::string path, ShaderProgram &s = *ShaderProgram::defaultShader);
    explicit ModelComponent(Model m, ShaderProgram &s = *ShaderProgram::defaultShader);
};

class TextComponent : public Component {
public:
    void update(float delta) override {};
    void draw() override;
    void draw(ShaderProgram &) override {};

    std::string text;
    glm::vec3 color = glm::vec3(0.0);
    Font *font;
    ShaderProgram shader;

    TextComponent(Font*, std::string, glm::vec3 = glm::vec3(0.0),
                  ShaderProgram& = *ShaderProgram::textShader);

    int assign(std::string name, glm::vec3 value) {
        if (name == "color") { color = value;}
        else { return 0;}
        return 1;
    }

    int assign(std::string name, std::string value) {
        if (name == "text") { text = value;}
        else { return 0;}
        return 1;
    }
};

class LightComponent : public Component {
public:
    Light *light;

    void update(float) override {
        Entity *ptr = Entity::get(entity);
        Transform *transform = ptr->transform();

        light->position = transform->position;
        light->rotation = transform->rotation;
    };
    void draw() override {};
    void draw(ShaderProgram&) override {};

    LightComponent(glm::vec3 c) {
        Light *l = new Light(glm::vec3(0.0), glm::vec3(0.0), c);
        light = l;
    };

    int assign(std::string name, glm::vec3 v) {
        if (name == "color") {light->color = v; return 1;}
        else {return 0;}
    }
};

#endif //PIPE_COMPONENTS_H
