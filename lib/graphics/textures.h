
#ifndef PIPE_TEXTURES_H
#define PIPE_TEXTURES_H

#include "../imports.h"

class CubeMap {
public:
    void bind() {glBindTexture(GL_TEXTURE_CUBE_MAP, self);};
    explicit CubeMap(std::vector<std::string> faces);
    CubeMap() {self = 0;};
private:
    unsigned int self;
};

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
private:
    unsigned int self;
    unsigned char* data;
};

class Texture2DArray {
public:
    void activate(int num) const;
    void bind() const;
    void unbind() const;

    unsigned int id() const {return self;};

    Texture2DArray(int w, int h, int d, int t = GL_RGB, int dtype=GL_UNSIGNED_BYTE);
    Texture2DArray(unsigned int target);
private:
    unsigned int self;
};


class FrameBuffer {
public:
    void bind() const;
    static void unbind();
    unsigned int id() const {return self;};
    void attachTexture2D(int attachment, Texture2D texture) const;
    void attachTexture2DArray(int attachment, Texture2DArray texture) const;
    void detachTexture2D(int attachment) const;

    void checkComplete();

    FrameBuffer();
    explicit FrameBuffer(unsigned int num) {self = num;};
private:
    unsigned int self;
};

#endif //PIPE_TEXTURES_H
