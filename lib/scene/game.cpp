
#include "game.h"

using namespace std;

vector<Entity *> Entity::entities;
map<string, Entity *> Entity::entityMap;

void Entity::update(float delta) {
    if (!active) {
        return;
    }
    for (auto component : components) {
        component->entity = this;
        component->update(delta);
    }
}

void Entity::draw() {
    if (!active) {
        return;
    }
    for (auto component : components) {
        component->draw();
    }
}

void Entity::draw(ShaderProgram &s) {
    if (!active) {
        return;
    }
    for (auto component : components) {
        component->draw(s);
    }
}
