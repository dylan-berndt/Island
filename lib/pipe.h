

#ifndef PIPE_PIPE_H
#define PIPE_PIPE_H

#include "imports.h"
#include "components.h"

void loadScene(std::string sceneName);

void initialize();

void update(float &delta);

void command(std::string);

std::vector<glm::mat4> getLightSpaceMatrices();

class World {
public:
    static Camera camera;
    static glm::vec3 lightDirection;
    static glm::vec3 lightColor;
    static SkyBox *skybox;
};

class Window {
public:
    static int width;
    static int height;
    static std::string name;
    static GLFWwindow* self;

    static void draw();

    static std::function<void(std::string)> userCommand;
    static void setUserCommand(std::function<void(std::string)>func) {
        userCommand = func;
    }

    static std::function<void(GLFWwindow*, int, int, int, int)> userKeyCallback;
    static void setUserKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> func) {
        userKeyCallback = func;
    }

    static std::function<void(GLFWwindow*)> userKeyPress;
    static void setUserKeyPress(std::function<void(GLFWwindow*)> func) {
        userKeyPress = func;
    }

    static void frameBufferSizeCallback(GLFWwindow *win, int width, int height);
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void charCallback(GLFWwindow *window, unsigned int);
    static void keyPress(GLFWwindow *window);
    static void mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);

    static void initializePostProcessing();
    static void attachPostProcessingTexture(Texture2D tex, int attachment) {
        postProcessingBuffer->attachTexture2D(attachment, tex);
    };

    static void blit() {
        screen->draw(*ShaderProgram::postShader);
    };

    static Font *defaultFont;

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
