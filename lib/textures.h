
#ifndef PIPE_TEXTURES_H
#define PIPE_TEXTURES_H

#include "back.h"

class Texture2D {
public:
    void activate(int num) const;
    void create(int w, int h, int t = GL_RGB, unsigned char* d = nullptr, int dtype=GL_UNSIGNED_BYTE);
    void bind() const;
    static void unbind();

    int width;
    int height;
    glm::vec3 offset = glm::vec3(0.0);
    glm::vec3 scale = glm::vec3(1.0);
    float bumpMultiplier = 1.0;

    unsigned int id() const {return self;};
    Texture2D(int w, int h, int t = GL_RGB, int dtype=GL_UNSIGNED_BYTE);
    explicit Texture2D(const char* name);
    explicit Texture2D(unsigned int num);
    Texture2D() {width = 0; height = 0; self = 0; data = nullptr;};

    std::string type;
    std::string path;
private:
    unsigned int self;
    unsigned char* data;
};


class FrameBuffer {
public:
    void bind() const;
    static void unbind();
    unsigned int id() const {return self;};
    void attachTexture2D(int attachment, Texture2D texture) const;
    void detachTexture2D(int attachment) const;
    FrameBuffer();
    explicit FrameBuffer(unsigned int num) {self = num;};
private:
    unsigned int self;
};

#endif //PIPE_TEXTURES_H
