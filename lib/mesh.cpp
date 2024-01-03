
#include "mesh.h"

using namespace std;

Mesh::Mesh(vector<Vertex> v, vector<int> i, vector<Texture2D> t) {
    vertices = v;
    indices = i;
    textures = t;

    model = glm::mat4(1.0);

    setupMesh();
}

void Mesh::draw(ShaderProgram &shader) {
    glBindVertexArray(VAO);

    shader.setMat4("model", model);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Mesh::draw() {
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

Mesh flatMesh(int nx, int ny) {
    vector<Vertex> vertices;
    vector<int> indices;

    for (int pz = 0; pz < ny; pz++) {
        for (int px = 0; px < nx; px++) {
            float x = float(px) / (float(ny) / 2.0f) - 1.0f;
            float y = 0.0;
            float z = float(pz) / (float(ny) / 2.0f) - 1.0f;

            vertices.push_back(Vertex(x, y, z));
        }
    }

    for (int z = 0; z < ny - 1; z++) {
        for (int x = 0; x < nx - 1; x++) {
            int tl = z * nx + x;
            int tr = tl + 1;
            int bl = (z + 1) * nx + x;
            int br = bl + 1;
            indices.push_back(tl); indices.push_back(bl); indices.push_back(tr);
            indices.push_back(tr); indices.push_back(bl); indices.push_back(br);
        }
    }

    return Mesh(vertices, indices, vector<Texture2D>());
}
