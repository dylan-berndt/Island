

#include "shader.h"

using namespace std;

glm::mat4 ShaderProgram::projection;
glm::mat4 ShaderProgram::view;
glm::vec3 ShaderProgram::lightDirection;
glm::vec3 ShaderProgram::lightColor;
glm::vec3 ShaderProgram::camera;
int ShaderProgram::width;
int ShaderProgram::height;

float ShaderProgram::nearPlane;
float ShaderProgram::farPlane;

ShaderProgram *ShaderProgram::defaultShader = nullptr;
ShaderProgram *ShaderProgram::shadowShader = nullptr;
ShaderProgram *ShaderProgram::skyboxShader = nullptr;
ShaderProgram *ShaderProgram::postShader = nullptr;
ShaderProgram *ShaderProgram::textShader = nullptr;

ShaderProgram::ShaderProgram() {
    unsigned int s;
    s = glCreateProgram();
    self = s;
}

ShaderProgram::ShaderProgram(string path) {
    path = path;
    name = path.substr(path.find_last_of('/') + 1);

    unsigned int s;
    s = glCreateProgram();
    self = s;

    map<string, int> shaderTypes = {
            pair<string, int> (".frag", GL_FRAGMENT_SHADER),
            pair<string, int> (".vert", GL_VERTEX_SHADER),
            pair<string, int> (".geom", GL_GEOMETRY_SHADER),
            pair<string, int> (".compute", GL_COMPUTE_SHADER)
    };

    for (auto type : shaderTypes) {
        ifstream file(path + type.first);

        if (file.good()) {
            #ifdef DEBUG
                Log << "Loaded shader " << path + type.first << endl;
            #endif
            openShader(path + type.first, type.second);
        }

        file.close();
    }

    compile();
}

void ShaderProgram::openShader(string name, int type) const {
    ifstream file(name);
    string source((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    string path = File::getPath("Resources/insert.txt");
    ifstream insertFile(path);
    string insertSource((istreambuf_iterator<char>(insertFile)), istreambuf_iterator<char>());

    int versionPosition = source.find_first_of('#');
    string sourceAfterVersion = source.substr(versionPosition);

    int newLinePosition = sourceAfterVersion.find_first_of('\n');
    source.insert(versionPosition + newLinePosition, "\n" + insertSource);

    const char *data = source.c_str();

    unsigned int shader;
    shader = glCreateShader(type);

    glShaderSource(shader, 1, &data, nullptr);
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

        cerr << "ERROR::SHADER Compilation of " << name << " failed" << endl;
        for (GLchar i: errorLog) {
            cerr << i;
        }

        glDeleteShader(shader);
        exit(-1);
    }
}

void ShaderProgram::compile() const {
    glLinkProgram(self);

    GLint isLinked = 0;
    glGetProgramiv(self, GL_LINK_STATUS, &isLinked);
    if(isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(self, GL_INFO_LOG_LENGTH, &maxLength);

        vector<GLchar> errorLog(maxLength);
        glGetProgramInfoLog(self, maxLength, &maxLength, &errorLog[0]);

        cerr << "ERROR::SHADER Linking failed" << endl;
        for (GLchar i: errorLog) {
            cerr << i;
        }

        glDeleteShader(self);
        exit(-1);
    }
}

void ShaderProgram::use() {
    if (used) {
        return;
    }
    used = true;

    glUseProgram(self);

    setInt("width", ShaderProgram::width);
    setInt("height", ShaderProgram::height);
    setMat4("projection", ShaderProgram::projection);
    setMat4("view", ShaderProgram::view);
    setVec3("camera", ShaderProgram::camera);
    setVec3("lightDirection", ShaderProgram::lightDirection);
    setVec3("lightColor", ShaderProgram::lightColor);
    setFloat("time", float(glfwGetTime()));

    setInt("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
    {
        setFloat("cascadePlaneDistances[" + std::to_string(i) + "]",
                             farPlane / shadowCascadeLevels[i]);
    }
}

void ShaderProgram::stop() {
    glUseProgram(0);
    used = false;
}

void ShaderProgram::setBool(const string& n, bool value) const {
    glUniform1i(glGetUniformLocation(self, n.c_str()), (int)value);
}

void ShaderProgram::setInt(const string& n, int value) const {
    glUniform1i(glGetUniformLocation(self, n.c_str()), value);
}

void ShaderProgram::setFloat(const string& n, float value) const {
    glUniform1f(glGetUniformLocation(self, n.c_str()), value);
}

void ShaderProgram::setVec2(const string& n, glm::vec2 value) const {
    glUniform2fv(glGetUniformLocation(self, n.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setVec3(const string& n, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(self, n.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const string& n, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(self, n.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
