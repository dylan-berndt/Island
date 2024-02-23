

#ifndef PIPE_SHADER_H
#define PIPE_SHADER_H

#include "../imports.h"

class Light {
public:
    static std::vector<Light> lights;

    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 rotation;

    float constant = 1.0;
    float linear = 0.0;
    float quadratic = 0.0;

    Light(glm::vec3 c, glm::vec3 p, glm::vec3 r) {
        color = c; position = p; rotation = r;
        lights.push_back(*this);
    }
};

class ShaderProgram {
public:
    static ShaderProgram *defaultShader;
    static ShaderProgram *shadowShader;
    static ShaderProgram *skyboxShader;
    static ShaderProgram *postShader;
    static ShaderProgram *textShader;

    static std::map<std::string, ShaderProgram> shaders;

    bool used = false;

    std::string name;

    static int width;
    static int height;

    static glm::mat4 projection;
    static glm::mat4 view;

    static glm::vec3 lightDirection;
    static glm::vec3 lightColor;

    static glm::vec3 camera;

    static int shadowMap;
    static std::vector<float> shadowCascadeLevels;

    static float nearPlane;
    static float farPlane;

    void openShader(std::string n, int type);
    void compile();
    void use();
    void stop();

    int assign(const std::string& n, int value) const;
    int assign(const std::string& n, float value) const;
    int assign(const std::string& n, glm::vec2 value) const;
    int assign(const std::string& n, glm::vec3 value) const;
    int assign(const std::string& n, glm::vec4 value) const;
    int assign(const std::string& n, glm::mat4 value) const;
    int assign(const std::string& n, std::string s) const {return 0;};

    ShaderProgram();
    ShaderProgram(std::string location);
private:
    unsigned int self;
};

#endif //PIPE_SHADER_H
