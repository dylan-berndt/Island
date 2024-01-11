
#include "obj.h"

#include <utility>
#include "stb_image.h"

using namespace std;


Cube::Cube() : Mesh(vector<Vertex>(), vector<int>()) {
    vector<Vertex> v;

    for (int p = 0; p < 8; p++) {
        float x = float(p % 2) * 2.0f - 1.0f;
        float y = float((p / 2) % 2) * 2.0f - 1.0f;
        float z = float((p / 4) % 2) * 2.0f - 1.0f;
        v.emplace_back(x, y, z);
    }

    vector<int> i {
            2, 6, 7, 7, 3, 2,
            5, 4, 0, 0, 1, 5,
            6, 2, 0, 0, 4, 6,
            1, 3, 7, 7, 5, 1,
            0, 2, 3, 3, 1, 0,
            7, 6, 4, 4, 5, 7
    };

    resetMesh(v, i);
}

Square::Square() : Mesh(vector<Vertex>(), vector<int>()) {
    vector<Vertex> v = {
            Vertex(glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0)),
            Vertex(glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0)),
            Vertex(glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0)),
            Vertex(glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0))
    };

    vector<int> i = {2, 1, 0, 2, 3, 1};

    resetMesh(v, i);
}

CubeMap::CubeMap(vector<string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    self = textureID;
}

SkyBox::SkyBox(std::vector<std::string> faces) : Cube() {
    cubeMap = CubeMap(faces);
}

void SkyBox::draw(ShaderProgram &shader) {
    glDepthMask(GL_FALSE);
    glCullFace(GL_FRONT);

    shader.use();

    glm::mat4 view = glm::mat4(glm::mat3(ShaderProgram::view));
    shader.setMat4("view", view);

    shader.setMat4("model", model);

    bind();

    cubeMap.bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    shader.stop();

    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
}
