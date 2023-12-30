
#define STB_IMAGE_IMPLEMENTATION
#include "back.h"
#include "textures.h"

using namespace std;

Texture2D::Texture2D(const char* name, int t, int w, int h, int dtype) {
    unsigned char *d = nullptr;
    int c;
    if (name != nullptr && name[0] != '\0') {
        d = stbi_load(name, &w, &h, &c, 0);
    }

    width = w;
    height = h;
    data = d;

    unsigned int texture;
    glGenTextures(1, &texture);

    self = texture;

    bind();

    glTexImage2D(GL_TEXTURE_2D, 0, t, width, height, 0, t, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind();
}

Texture2D::Texture2D(int num) {
    self = num;
    data = nullptr;

    bind();

    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    width = w; height = h;

    unbind();
}

void Texture2D::activate(int num) const {
    glActiveTexture(num);
    bind();
}

void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, self);
}

void Texture2D::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

FrameBuffer::FrameBuffer() {
    unsigned int buffer;
    glGenFramebuffers(1, &buffer);
    self = buffer;

    bind();

    unbind();
}

void FrameBuffer::attachTexture2D(int attachment, Texture2D texture) const {
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.id(), 0);
    unbind();
}

void FrameBuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, self);
}

void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
