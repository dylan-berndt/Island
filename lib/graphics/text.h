
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
    int lineHeight = 0;

    Font(std::string fileName, int fontSize = 12);
    void initialize(int);

    void render(std::string text, float x, float y, glm::vec3 = glm::vec3(1.0), glm::vec3 = glm::vec3(0.0),
                ShaderProgram &shader = *ShaderProgram::textShader, float wrap = -1);

private:
    DynamicMesh mesh;
    FT_Face face;
};

#endif //PIPE_TEXT_H
