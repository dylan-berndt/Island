

#include "back.h"
#include "shader.h"

using namespace std;

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

    setMat4("projection", ShaderProgram::perspective);
    setMat4("view", ShaderProgram::view);
    setVec3("camera", ShaderProgram::camera);
    setFloat("time", float(glfwGetTime()));
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

void ShaderProgram::setVec3(const string& name, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(self, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const string& name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(self, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
