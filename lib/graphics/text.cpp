
#include "text.h"

using namespace std;

Font::Font(string path, int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        cerr << "ERROR::FREETYPE Could not init FreeType Library" << endl;
    }

    FT_Face f;
    if (FT_New_Face(ft, path.c_str(), 0, &f))
    {
        cerr << "ERROR::FREETYPE Failed to load font " << path << endl;
    }

    face = f;

    initialize(fontSize);

    FT_Done_FreeType(ft);
}

void Font::initialize(int fontSize) {
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 127; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            cerr << "ERROR::FREETYPE Failed to load Glyph " << c << endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );

//        cout << int(c) << " " << c
//        << " " << face->glyph->bitmap.width
//        << " " << face->glyph->bitmap.rows << endl;
//
//        for (int y = 0; y < face->glyph->bitmap.rows; y++) {
//            for (int x = 0; x < face->glyph->bitmap.width; x++) {
//                int val = int(face->glyph->bitmap.buffer[x + y * face->glyph->bitmap.width]);
//                char d = (val > 0) ? 'X' : ' ';
//                cout << d;
//            }
//            cout << endl;
//        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
        };
        characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
}

void Font::render(string text, float x, float y, float scale, glm::vec3 color) {
    ShaderProgram::textShader->use();
    ShaderProgram::textShader->setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);

}











