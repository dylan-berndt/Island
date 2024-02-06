
#ifndef PIPE_TEXT_H
#define PIPE_TEXT_H

#include "../imports.h"
#include "mesh.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};


class Font {
public:
    std::map<char, Character> characters;

    Font(std::string fileName, int fontSize = 12);
    void initialize(int);

    void render(std::string text, float x, float y, float = 1.0, glm::vec3 = glm::vec3(0.0));

private:
    FT_Face face;
};

#endif //PIPE_TEXT_H
