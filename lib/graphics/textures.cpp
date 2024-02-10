
#define STB_IMAGE_IMPLEMENTATION
#include "../imports.h"
#include "textures.h"

#include "stb_image.h"

using namespace std;

CubeMap::CubeMap(string path) {
    map<int, string> order;
    vector<string> directions = {"lf", "rt", "up", "dn", "ft", "bk"};

    for (const auto& entry : filesystem::directory_iterator(path)) {
        for (int i = 0; i < directions.size(); i++) {
            string find = directions[i];
            string name = entry.path().string();
            string end = name.substr(name.find_last_of('/'));
            if (end.find(find) != string::npos) {
                order.insert(pair<int, string>(i, name));
                break;
            }
        }
    }

    vector<string> faces;

    for (int i = 0; i < 6; i++) {
        faces.push_back(order[i]);
    }

    create(faces);
}

CubeMap::CubeMap(vector<string> faces) {
    create(faces);
}

void CubeMap::create(vector<string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        int t = GL_RGB;
        bool RGBA = false;
        if (faces[i].length() >= 4) {
            if (faces[i].find(".png") != faces[i].npos) {
                t = GL_RGBA;
                RGBA = true;
            }
            if (faces[i].find(".tga") != faces[i].npos) {
                t = GL_BGRA;
                RGBA = true;
            }
        }
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, RGBA ? STBI_rgb_alpha : STBI_rgb);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA, width, height, 0, t, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            Log << "\aERROR::CUBEMAP " << faces[i] << " failed to load\a" << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    self = textureID;
}

Texture2D::Texture2D(int w, int h, int t, int dtype) {
    data = nullptr;
    width = w;
    height = h;

    unsigned int texture;
    glGenTextures(1, &texture);

    self = texture;

    create(width, height, t, data, dtype);
}

Texture2D::Texture2D(string path) {
    int c, w, h, t;
    data = nullptr;

    #ifdef DEBUG
        Log << "Loaded texture " << path << endl;
    #endif

    stbi_set_flip_vertically_on_load(true);
    if (!path.empty() && path[0] != '\0') {
        int length = path.length();

        t = GL_RGB;
        bool RGBA = false;
        if (length >= 4) {
            if (path.find(".png") != path.npos) {
                t = GL_RGBA;
                RGBA = true;
            }
            if (path.find(".tga") != path.npos) {
                t = GL_BGRA;
                RGBA = true;
            }
        }

        unsigned char *d = stbi_load(path.c_str(), &w, &h, &c, RGBA ? STBI_rgb_alpha : STBI_rgb);
        data = d;

        if(stbi_failure_reason() && !data) {
            Log << "\aERROR::TEXTURE " << path << " " << stbi_failure_reason() << "\a" << endl;
        }
    }

    width = w;
    height = h;

    unsigned int texture;
    glGenTextures(1, &texture);

    self = texture;

    create(width, height, t, data, GL_UNSIGNED_BYTE);
}

void Texture2D::create(int w, int h, int t, unsigned char* d, int dtype) {
    bind();

    glTexImage2D(GL_TEXTURE_2D, 0, t, w, h, 0, t, dtype, d);
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

Texture2DArray::Texture2DArray(int w, int h, int d, int t, int dtype) {
    unsigned int texture;
    glGenTextures(1, &texture);

    self = texture;

    bind();
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, t, w, h, d, 0, t, dtype, nullptr);
    unbind();
}

Texture2DArray::Texture2DArray(unsigned int target) {
    self = target;
}

void Texture2DArray::activate(int num) const {
    glActiveTexture(num);
    bind();
}

void Texture2DArray::bind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, self);
}

void Texture2DArray::unbind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
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

void FrameBuffer::attachTexture2DArray(int attachment, Texture2DArray texture) const {
    bind();
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture.id(), 0);
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

void FrameBuffer::checkComplete() {
    bind();

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        if (status == GL_FRAMEBUFFER_UNDEFINED) {
            Log << "\aERROR:FRAMEBUFFER Undefined\a" << endl;
        }
        else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
            Log << "\aERROR::FRAMEBUFFER Incomplete attachment\a" << endl;
        }
        else if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
            Log << "\aERROR::FRAMEBUFFER Incomplete draw buffer\a" << endl;
        }
        else if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
            Log << "\aERROR::FRAMEBUFFER Incomplete read buffer\a" << endl;
        }
        else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
            Log << "\aERROR::FRAMEBUFFER Missing attachment\a" << endl;
        }
        else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
            Log << "\aERROR::FRAMEBUFFER Unsupported attachment\a" << endl;
        }
        else {
            Log << "\aERROR::FRAMEBUFFER No clue\a" << endl;
        }
    }
}
