
#include "components.h"

using namespace std;

Camera::Camera(glm::vec3 pos, glm::vec3 rot) {
    position = pos;
    rotation = rot;

    getRotation();
}

void Camera::rotateByMouse(double dx, double dy) {
    rotation.y += dx * 0.1;
    rotation.x += dy * 0.1;

    rotation.x = glm::clamp(rotation.x, -79.0f, 79.0f);

    getRotation();
}

glm::mat4 Camera::getView() {
    glm::mat4 view(1.0);

    view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    view = glm::translate(view, -position);

    return view;
}

glm::mat4 Camera::getProjection() {
    if (!orthographic) {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }
    else {
        return glm::ortho(-aspect * size, aspect * size, -size, size, nearPlane, farPlane);
    }
}

void Camera::getRotation() {
    glm::mat4 view(1.0);
    view = glm::rotate(view, glm::radians(-rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(-rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    forward = glm::vec3(view * glm::vec4(glm::vec3(0.0, 0.0, -1.0), 0.0));
    up = glm::vec3(view * glm::vec4(glm::vec3(0.0, 1.0, 0.0), 0.0));
    right = glm::vec3(view * glm::vec4(glm::vec3(1.0, 0.0, 0.0), 0.0));
}

void ModelComponent::update(float delta) {
    Entity *ptr = Entity::get(entity);
    for (int i = 0; i < model.meshes.size(); i++) {
        model.meshes[i].model = ptr->transform()->matrix;
    }
}

void ModelComponent::draw() {
    if (!cullFace) {
        glDisable(GL_CULL_FACE);
    }
    shader.use();
    model.draw(shader);
    shader.stop();
    if (!cullFace) {
        glEnable(GL_CULL_FACE);
    }
}

void ModelComponent::draw(ShaderProgram &s) {
    if (!cullFace) {
        glDisable(GL_CULL_FACE);
    }
    model.draw(s);
    if (!cullFace) {
        glEnable(GL_CULL_FACE);
    }
}

ModelComponent::ModelComponent(Model m, ShaderProgram &s) : model(std::move(m)){
    shader = s;
}

ModelComponent::ModelComponent(string path, ShaderProgram &s) : model(path) {
    shader = s;
}

TextComponent::TextComponent(Font *f, std::string t, glm::vec3 c, ShaderProgram &s) {
    font = f;
    text = t;
    color = c;
    shader = s;
}

void TextComponent::draw() {
    Entity *ptr = Entity::get(entity);
    glm::vec3 scale = ptr->transform()->scale;
    glm::vec3 position = ptr->transform()->position;

    font->render(text, position.x, position.y, scale, color, shader);
}
