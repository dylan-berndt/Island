
#ifndef PIPE_TEXTURES_H
#define PIPE_TEXTURES_H

#include "back.h"

class Texture2D {
public:
    void activate(int num) const;
    void create(int w, int h, int t = GL_RGB, unsigned char* d = nullptr);
    void bind() const;
    static void unbind();
    int width;
    int height;
    unsigned int id() const {return self;};
    explicit Texture2D(const char* name, int w = 0, int h = 0, int t = GL_RGB);
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
