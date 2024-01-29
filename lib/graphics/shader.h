

#ifndef PIPE_SHADER_H
#define PIPE_SHADER_H

#include "../imports.h"

class ShaderProgram {
public:
    static ShaderProgram *defaultShader;
    static ShaderProgram *shadowShader;
    static ShaderProgram *skyboxShader;
    static ShaderProgram *postShader;

    bool used = false;

    std::string name;

    static int width;
    static int height;

    static glm::mat4 perspective;
    static glm::mat4 view;

    static glm::vec3 lightDirection;

    static glm::vec3 camera;

    void openShader(std::string name, int type) const;
    void compile() const;
    void use();
    void stop();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, glm::vec2 value) const;
    void setVec3(const std::string& name, glm::vec3 value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;

    ShaderProgram();
    ShaderProgram(std::string location);
private:
    unsigned int self;
};

#endif //PIPE_SHADER_H
