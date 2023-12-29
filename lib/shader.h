

#ifndef PIPE_SHADER_H
#define PIPE_SHADER_H

#include "back.h"

class ShaderProgram {
public:
    void openShader(const std::string& name, int type) const;
    void compile() const;
    void use() const;
    void stop() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, glm::vec3 value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;

    ShaderProgram();
private:
    unsigned int self;
};

#endif //PIPE_SHADER_H
