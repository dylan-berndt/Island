

#ifndef PIPE_PIPE_H
#define PIPE_PIPE_H

#include "imports.h"
#include "components.h"

void loadScene(std::string sceneName);

void initialize();

void update(float &delta);

std::vector<glm::mat4> getLightSpaceMatrices();

class Scene {
public:
    std::vector<Entity> entities;
    glm::vec3 lightDirection;
    Camera camera;

    Scene(const std::string sceneName);
    Scene() {entities = std::vector<Entity>(); lightDirection = glm::vec3(1.0);};
};

class World {
public:
    static Camera camera;
    static Scene scene;
    static glm::vec3 lightDirection;
};

class Window {
public:
    static int width;
    static int height;
    static std::string name;
    static GLFWwindow* self;

    static void draw();

    static void frameBufferSizeCallback(GLFWwindow *win, int width, int height);

    static void initializePostProcessing();
    static void attachPostProcessingTexture(Texture2D tex, int attachment) {
        postProcessingBuffer->attachTexture2D(attachment, tex);
    };

    static void blit() {
        screen->draw(*ShaderProgram::postShader);
    };

private:
    static Square *screen;
    static FrameBuffer *postProcessingBuffer;
    static FrameBuffer *shadowBuffer;
    static Texture2D *colorTexture;
    static Texture2D *depthTexture;
    static Texture2DArray *shadowTexture;

    static unsigned int lightMatricesUBO;
};

#endif //PIPE_PIPE_H
