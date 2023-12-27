
#include "back.h"
using namespace std;

float *flatMesh(float width, float depth, int nx, int nz, float height) {
    auto *mesh = new float[nx * nz * 3];

    for (int pz = 0; pz < nz; pz++) {
        for (int px = 0; px < nx; px++) {
            float x = width * (float(px) / (float(nz) / 2.0) - 1.0);
            float y = height;
            float z = depth * (float(pz) / (float(nz) / 2.0) - 1.0);

            int start = pz * nx + px * 3;
            mesh[start] = x; mesh[start + 1] = y; mesh[start + 2] = z;
        }
    }

    return mesh;
}

VertexArray::VertexArray(float *v, int size, int mode) {
    vertices = v;
    unsigned int a, b; VAO = a; VBO = b;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(float), vertices, mode);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::bind() const {
    glBindVertexArray(VAO);
}

unsigned int genWater(float width, float depth, int nx, int nz, float height) {
    float *mesh = flatMesh(width, depth, nx, nz, height);
    unsigned int water;
//    bindVertexArray(water, mesh);

    int indices[6 * (nx - 1) * (nz - 1)];
    int in = 0;
    for (int z = 0; z < nz - 1; z++) {
        for (int x = 0; x < nx - 1; x++) {
            int current = z * nx + x;
            int next    = (z + 1) * nx + x;
            indices[in++] = current; indices[in++] = current + 1; indices[in++] = next;
            indices[in++] = current + 1; indices[in++] = next + 1; indices[in++] = next;
        }
    }

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    return water;
}

GLchar *shaderString(const string& name) {
    ifstream file;
    char* src;
    GLsizei length;

    file.open(name);

    file.seekg(0, ios::end);
    length = file.tellg();

    src = new char[length];
    file.seekg(0, ios::beg);
    file.read(src, length);
    file.close();

    return src;
}

ShaderProgram::ShaderProgram() {
    unsigned int s;
    s = glCreateProgram();
    self = s;
}

void ShaderProgram::openShader(const string& name, int type) const {
    const GLchar *source = shaderString(name);

    unsigned int shader;
    shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    glAttachShader(self, shader);
}

void ShaderProgram::compile() const {
    glLinkProgram(self);
}

void ShaderProgram::use() const {
    glUseProgram(self);
}


