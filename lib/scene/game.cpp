
#include "game.h"

using namespace std;

vector<Entity *> Entity::entities;

void Entity::update(float delta) {
    for (auto component : components) {
        component->entity = this;
        component->update(delta);
    }
}

void Entity::draw() {
    for (auto component : components) {
        component->draw();
    }
}

void Entity::draw(ShaderProgram &s) {
    for (auto component : components) {
        component->draw(s);
    }
}
