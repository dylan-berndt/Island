
#ifndef PIPE_TEXTURES_H
#define PIPE_TEXTURES_H

#include "back.h"

class FrameBuffer {
public:
    FrameBuffer();
    void bind() const;
    void unbind() const;
private:
    unsigned int self;
};

class Texture2D {
public:
    Texture2D();
    void bind() const;
    void unbind() const;
private:
    unsigned int self;
};

#endif //PIPE_TEXTURES_H
