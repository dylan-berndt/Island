
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

TextComponent::TextComponent(Font *f, std::string t, glm::vec3 c, ShaderProgram &s) : mesh(vector<Vertex>(), vector<int>()) {
    font = f;
    text = t;
    color = c;
    shader = s;

    vector<Vertex> v = {
            Vertex(glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0)),
            Vertex(glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0)),
            Vertex(glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0)),
            Vertex(glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0))
    };

    vector<int> i = {0, 1, 2, 1, 3, 2};

    mesh.resetMesh(v, i);
}

void TextComponent::draw() {
    shader.use();

    shader.setVec3("textColor", color);

    glm::mat4 orthographic = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f);
    shader.setMat4("textProjection", orthographic);

    glActiveTexture(GL_TEXTURE0);

    mesh.bind();

    Entity *ptr = Entity::get(entity);
    glm::vec3 scale = ptr->transform()->scale;
    glm::vec3 position = ptr->transform()->position;

    float x = position.x;
    float y = position.y;

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = font->characters[*c];

        float xpos = x + ch.Bearing.x * scale.x;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale.y;

        float w = ch.Size.x * scale.x;
        float h = ch.Size.y * scale.y;

        vector<Vertex> vertices = {
                Vertex(glm::vec3(xpos, ypos, 0.0), glm::vec2(0.0, 1.0)),
                Vertex(glm::vec3(xpos + w, ypos, 0.0), glm::vec2(1.0, 1.0)),
                Vertex(glm::vec3(xpos, ypos + h, 0.0), glm::vec2(0.0, 0.0)),
                Vertex(glm::vec3(xpos + w, ypos + h, 0.0), glm::vec2(1.0, 0.0))
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        mesh.updateSubData(vertices);

        mesh.draw(shader);

        x += (ch.Advance >> 6) * scale.x;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    shader.stop();
}
