
#define STB_IMAGE_IMPLEMENTATION
#include "back.h"
#include "textures.h"

#include "stb_image.h"

using namespace std;

Texture2D::Texture2D(const char* name, int w, int h, int t, int dtype) {
    int c;
    data = nullptr;

    stbi_set_flip_vertically_on_load(true);
    if (name != nullptr && name[0] != '\0') {
        unsigned char *d = stbi_load(name, &w, &h, &c, 0);
        data = d;

        if(stbi_failure_reason()) {
            std::cout << "ERROR::TEXTURE " << name << " " << stbi_failure_reason() << endl;
        }
    }

    width = w;
    height = h;

    unsigned int texture;
    glGenTextures(1, &texture);

    self = texture;

    create(width, height, t, data);
}

void Texture2D::create(int w, int h, int t, unsigned char* d) {
    bind();

    glTexImage2D(GL_TEXTURE_2D, 0, t, w, h, 0, t, GL_UNSIGNED_BYTE, d);
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind();
}

Texture2D::Texture2D(unsigned int num) {
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

void FrameBuffer::detachTexture2D(int attachment) const {
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
    unbind();
}

void FrameBuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, self);
}

void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
