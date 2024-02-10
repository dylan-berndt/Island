
#include "text.h"

using namespace std;

Font::Font(string path, int fontSize) : mesh(vector<Vertex>(), vector<int>()) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        Log << "\aERROR::FREETYPE Could not init FreeType Library\a" << endl;
    }

    FT_Face f;
    if (FT_New_Face(ft, path.c_str(), 0, &f))
    {
        Log << "\aERROR::FREETYPE Failed to load font " << path << "\a" << endl;
    }

    face = f;

    initialize(fontSize);

    FT_Done_FreeType(ft);

    vector<Vertex> v = {
            Vertex(glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0)),
            Vertex(glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0)),
            Vertex(glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0)),
            Vertex(glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0))
    };

    vector<int> i = {0, 1, 2, 1, 3, 2};

    mesh.resetMesh(v, i);
}

void Font::initialize(int fontSize) {
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 127; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            Log << "\aERROR::FREETYPE Failed to load Glyph " << c << "\a" << endl;
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

    lineHeight = face->size->metrics.height;

    FT_Done_Face(face);
}

void Font::render(string text, float x, float y, glm::vec3 scale, glm::vec3 color, ShaderProgram &shader) {
    shader.use();

    float left = x;
    bool alert = false;

    shader.setVec3("textColor", color);
    glm::mat4 orthographic = glm::ortho(0.0f, float(ShaderProgram::width), 0.0f, float(ShaderProgram::height));
    shader.setMat4("textProjection", orthographic);

    glActiveTexture(GL_TEXTURE0);

    mesh.bind();

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        if (*c == '\n') {
            x = left;
            y -= (lineHeight >> 6) * scale.y;
            continue;
        }

        if (*c == '\a') {
            alert = !alert;
            shader.setVec3("textColor", alert ? glm::vec3(1.0, 0.0, 0.0) : color);
            continue;
        }

        Character ch = characters[*c];

        float xpos = x + ch.Bearing.x * scale.x;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale.y;

        float w = ch.Size.x * scale.x;
        float h = ch.Size.y * scale.y;

        vector<Vertex> vertices = {
                Vertex(glm::vec3(xpos, ypos, 0.0), glm::vec2(0.0, 1.0)),
                Vertex(glm::vec3(xpos + w, ypos, 0.0), glm::vec2(1.0, 1.0)),
                Vertex(glm::vec3(xpos, ypos + h, 0.0), glm::vec2(0.0, 0.0)),
                Vertex(glm::vec3(xpos + w, ypos + h, 0.0), glm::vec2(1.0, 0.0))
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        mesh.updateSubData(vertices);

        mesh.draw(shader);

        x += (ch.Advance >> 6) * scale.x;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    shader.stop();
}











