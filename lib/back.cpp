
#include "back.h"
using namespace std;

float *flatMesh(float width, float depth, int nx, int nz, float height) {
    auto *mesh = new float[nx * nz * 3];

    int start = 0;
    for (int pz = 0; pz < nz; pz++) {
        for (int px = 0; px < nx; px++) {
            float x = width * (float(px) / (float(nz) / 2.0f) - 1.0f);
            float y = height;
            float z = depth * (float(pz) / (float(nz) / 2.0f) - 1.0f);

            mesh[start] = x; mesh[start + 1] = y; mesh[start + 2] = z;
            start += 3;
        }
    }

    return mesh;
}

IndexedArray genWater(float width, float depth, int nx, int nz, float height) {
    float *mesh = flatMesh(width, depth, nx, nz, height);

    unsigned int indices[6 * (nx - 1) * (nz - 1)];
    int in = 0;
    for (int z = 0; z < nz - 1; z++) {
        for (int x = 0; x < nx - 1; x++) {
            int tl = z * nx + x;
            int tr = tl + 1;
            int bl = (z + 1) * nx + x;
            int br = bl + 1;
            indices[in++] = tl; indices[in++] = bl; indices[in++] = tr;
            indices[in++] = tr; indices[in++] = bl; indices[in++] = br;
        }
    }

    IndexedArray v(mesh, nx * nz, indices, 6 * (nx - 1) * (nz - 1));

    return v;
}

VertexArray::VertexArray(float *v, int size, int mode) {
    vertices = v;
    amount = size / 3;
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

void VertexArray::draw() const {
    bind();
    glDrawArrays(GL_TRIANGLES, 0, amount);
    unbind();
}

IndexedArray::IndexedArray(float *v, int vs, unsigned int *i, int is, int mode) {
    vertices = v;
    indices = i;
    amount = is;
    unsigned int a, b, e; VAO = a; VBO = b; EBO = e;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vs * 3 * sizeof(float), vertices, mode);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, is * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void IndexedArray::unbind() {
    glBindVertexArray(0);
}

void IndexedArray::bind() const {
    glBindVertexArray(VAO);
}

void IndexedArray::draw() const {
    bind();
    glDrawElements(GL_TRIANGLES, amount, GL_UNSIGNED_INT, 0);
    unbind();
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

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        cerr << "Shader error: Compilation of " << name << " failed" << endl;
        for (GLchar i: errorLog) {
            cout << i;
        }

        glDeleteShader(shader);
        exit(-1);
    }

}

void ShaderProgram::compile() const {
    glLinkProgram(self);
}

void ShaderProgram::use() const {
    glUseProgram(self);
}

void ShaderProgram::stop() const {
    glUseProgram(0);
}

void ShaderProgram::setBool(const string& name, bool value) const {
    glUniform1i(glGetUniformLocation(self, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const string& name, int value) const {
    glUniform1i(glGetUniformLocation(self, name.c_str()), value);
}

void ShaderProgram::setFloat(const string& name, float value) const {
    glUniform1f(glGetUniformLocation(self, name.c_str()), value);
}

void ShaderProgram::setMat4(const string& name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(self, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}


