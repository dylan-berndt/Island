
#include "imports.h"

std::string File::resourceLocation;
std::stringstream Log;
std::stringstream Commands;

std::ostream& operator<< (std::ostream& os, glm::vec1 v) {
    os << v.x;
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::vec2 v) {
    os << v.x << ", " << v.y;
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::vec3 v) {
    os << v.x << ", " << v.y << ", " << v.z;
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::vec4 v) {
    os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::mat3 v) {
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            os << v[y][x] << " ";
        }
        os << std::endl;
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::mat4 v) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            os << v[y][x] << " ";
        }
        os << std::endl;
    }
    return os;
}

std::string File::getPath(std::string path) {
    return resourceLocation + path;
}

int glError() {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        Log << "\aOpenGL error: " << err << "\a" << std::endl;
        return 1;
    }
    return 0;
}

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    Log << "\aGL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
        << " type = 0x" << type
        << ", severity = 0x" << severity
        << ", message = " << message
        << "\a" << std::endl;
}

