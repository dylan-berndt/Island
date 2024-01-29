
#ifndef PIPE_GAME_H
#define PIPE_GAME_H

#include "../imports.h"
#include "../graphics/shader.h"


class Component {
public:
    virtual void update(float delta) {};
    virtual void draw() {};
    virtual void draw(ShaderProgram &) {};
    void* entity = nullptr;
};

class Transform : public Component {
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

    void update(float delta) {
        calcMatrix();
    }

    Transform(glm::vec3 p, glm::vec3 r, glm::vec3 s) {
        position = p; rotation = r; scale = s;
        calcMatrix();
    };
    Transform() {
        position = glm::vec3(0.0); rotation = glm::vec3(0.0); scale = glm::vec3(1.0);
        calcMatrix();
    };
    Transform(glm::vec3 p) {
        position = p; rotation = glm::vec3(0.0); scale = glm::vec3(1.0);
        calcMatrix();
    };
};

class Entity {
public:
    std::string name;
    std::vector<Component *> components;
    bool active = true;

    void update(float delta);
    void draw();
    void draw(ShaderProgram &);

    void addComponent(Component *c) {
        components.push_back(c);
    };

    template <typename T>
    T* getComponent() {
        for (auto component : components) {
            try {
                return dynamic_cast<T *>(component);
            }
            catch (const std::string& ex) {

            }
        }
        return nullptr;
    };

    Transform* transform() {
        return getComponent<Transform>();
    };

    Entity(std::string n, Transform *t = nullptr, bool tracked = false) {
        name = n;

        if (t == nullptr) {
            t = new Transform();
        }

        components.push_back(t);

        if (!tracked) {
            entities.push_back(this);
        }
    };

    static Entity* get(void *ptr) {
        auto* entityPtr = dynamic_cast<Entity*>(static_cast<Entity*>(ptr));
        return entityPtr;
    };

    ~Entity() {
        for (auto component : components) {
//            delete component;
        }
    }

    static std::vector<Entity *> entities;
};

#endif //PIPE_GAME_H
