
#ifndef PIPE_TEXTURES_H
#define PIPE_TEXTURES_H

#include "back.h"

class Texture2D {
public:
    void activate(int num) const;
    void bind() const;
    static void unbind();
    int width;
    int height;
    unsigned int id() const {return self;};
    explicit Texture2D(const char* name, int t = GL_RGBA, int w = 0, int h = 0, int dtype = GL_UNSIGNED_BYTE);
    explicit Texture2D(int num);
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
    FrameBuffer();
private:
    unsigned int self;
};

#endif //PIPE_TEXTURES_H
