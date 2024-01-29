
#include "pipe.h"

using namespace std;

int Window::width;
int Window::height;
GLFWwindow *Window::self;
std::string Window::name;

FrameBuffer *Window::postProcessingBuffer = nullptr;
FrameBuffer *Window::shadowBuffer = nullptr;
Texture2D *Window::colorTexture = nullptr;
Texture2D *Window::depthTexture = nullptr;
Texture2DArray *Window::shadowTexture;
Square *Window::screen = nullptr;
unsigned int Window::lightMatricesUBO;

Scene World::scene;
Camera World::camera;
glm::vec3 World::lightDirection;

int shadowMapResolution = 2048;
vector<float> shadowCascadeLevels;
float nearPlane, farPlane;

float previousFrameTime = 0.0;

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );
}

Transform *transformFromString(string transformationData) {
    glm::vec3 transforms[3];

    for (int i = 0; i < 3; i++) {
        transforms[i] = glm::vec3(i < 2 ? 0.0 : 1.0);

        size_t left = transformationData.find_first_of('(');
        size_t right = transformationData.find_first_of(')');

        if (left == string::npos) {
            continue;
        }

        istringstream current(transformationData.substr(left + 1, right - left - 1));

        float x, y, z;
        current >> x;

        transforms[i] = glm::vec3(x);

        if (current >> y) {
            current >> z;

            transforms[i] = glm::vec3(x, y, z);
        }

        transformationData = transformationData.substr(right + 1);
    }

    return new Transform(transforms[0], transforms[1], transforms[2]);
}

Scene::Scene(string sceneName) {
    ifstream scene(sceneName);

    entities = vector<Entity>();
    lightDirection = glm::vec3(1.0);

    cout << "Loading scene " << sceneName << endl;

    string line;
    while (getline(scene, line)) {
        string type;
        istringstream iss(line);
        iss >> type;

        if (type == "L") {
            float x, y, z;
            iss >> x >> y >> z;
            lightDirection = glm::normalize(glm::vec3(x, y, z));
        }
        if (type == "C") {
            float x, y, z, rx, ry, rz;
            iss >> x >> y >> z >> rx >> ry >> rz;
            camera = Camera(glm::vec3(x, y, z), glm::vec3(rx, ry, rz));
        }
        if (type == "M") {
            string name;
            iss >> name;

            string path;
            string section;
            iss >> section;
            path += section;
            while (iss >> section && section[0] != '(') {
                path += " " + section;
            }

            cout << "Loading model " << path << endl;

            ModelComponent *modelComponent = new ModelComponent(path);

            string transformationData;
            getline(iss,  transformationData);

            Transform *transform = transformFromString(section + transformationData);

            Entity newModel(name, transform, true);
            newModel.addComponent(modelComponent);

            entities.push_back(newModel);
        }
    }

    scene.close();
}

void loadScene(string sceneName) {
    Scene current(sceneName);

    World::scene = current;
    World::lightDirection = current.lightDirection;
    World::camera = current.camera;
}

void initialize(string configName, string sceneName) {
    ifstream config(configName);

    config >> Window::width >> Window::height >> Window::name;
    float fov;
    config >> fov >> nearPlane >> farPlane;
    ShaderProgram::perspective = glm::perspective(glm::radians(fov),
                                                  float(Window::width) / float(Window::height),
                                                  nearPlane, farPlane);

    shadowCascadeLevels = {farPlane / 50.0f, farPlane / 25.0f, farPlane / 5.0f, farPlane / 2.0f};

    if (!glfwInit()) {
        cout << "Can't Initialize GLFW" << endl;
        exit (EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *win;
    win = glfwCreateWindow(Window::width, Window::height, Window::name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(win);

    Window::self = win;

    gladLoadGL();

    glViewport(0, 0, Window::width, Window::width);

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    config.close();

    ShaderProgram::defaultShader = new ShaderProgram("../Resources/default");
    ShaderProgram::shadowShader = new ShaderProgram("../Resources/shadow");
    ShaderProgram::skyboxShader = new ShaderProgram("../Resources/skybox");
    ShaderProgram::postShader = new ShaderProgram("../Resources/defaultPost");

    Window::initializePostProcessing();

    glfwSetFramebufferSizeCallback(Window::self, Window::frameBufferSizeCallback);

    loadScene(sceneName);
}

void update(float &delta) {
    delta = glfwGetTime() - previousFrameTime;
    previousFrameTime = glfwGetTime();

    ShaderProgram::camera = World::camera.position;
    ShaderProgram::view = World::camera.getView();
    ShaderProgram::lightDirection = World::lightDirection;

    for (auto entity : World::scene.entities) {
        entity.update(delta);
    }
    for (auto entity : Entity::entities) {
        entity->update(delta);
    }
}

void Window::draw() {
    const auto lightMatrices = getLightSpaceMatrices();
    glBindBuffer(GL_UNIFORM_BUFFER, lightMatricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shadowBuffer->bind();
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    ShaderProgram::shadowShader->use();

    for (auto entity : World::scene.entities) {
        entity.draw(*ShaderProgram::shadowShader);
    }

    ShaderProgram::shadowShader->stop();

    glCullFace(GL_BACK);
    shadowBuffer->unbind();

    glViewport(0, 0, Window::width, Window::height);

    postProcessingBuffer->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto entity : Entity::entities) {
        entity->draw();
    }
    for (auto entity : World::scene.entities) {
        entity.draw();
    }

    postProcessingBuffer->unbind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderProgram::postShader->use();

    ShaderProgram::postShader->setInt("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
    {
        ShaderProgram::postShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }

    blit();

    ShaderProgram::postShader->stop();
}

void Window::frameBufferSizeCallback(GLFWwindow *win, int width, int height) {
    colorTexture->create(width, height, GL_RGB);
    depthTexture->create(width, height, GL_DEPTH_COMPONENT, nullptr, GL_FLOAT);
    Window::width = width;
    Window::height = height;
}

void Window::initializePostProcessing() {
    colorTexture = new Texture2D(width, height, GL_RGB);
    depthTexture = new Texture2D(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);

    postProcessingBuffer = new FrameBuffer();
    postProcessingBuffer->attachTexture2D(GL_COLOR_ATTACHMENT0, *colorTexture);
    postProcessingBuffer->attachTexture2D(GL_DEPTH_ATTACHMENT, *depthTexture);

    shadowBuffer = new FrameBuffer();

    unsigned int shadowArray;
    glGenTextures(1, &shadowArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowArray);
    glTexImage3D(
            GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,  shadowMapResolution, shadowMapResolution, int(shadowCascadeLevels.size()) + 1,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    shadowTexture = new Texture2DArray(shadowArray);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    shadowBuffer->bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowArray, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    shadowTexture->unbind();

    shadowBuffer->checkComplete();

    glGenBuffers(1, &lightMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightMatricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    screen = new Square();

    screen->material.setTexture2D("color", *colorTexture);
    screen->material.setTexture2D("depth", *depthTexture);
    screen->material.setTexture2DArray("shadowMap", *shadowTexture);
}

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

glm::mat4 getLightSpaceMatrix(const float localNear, const float localFar)
{
    const auto proj = glm::perspective(
            glm::radians(60.0f), (float)Window::width / (float)Window::height, localNear, localFar);
    const auto corners = getFrustumCornersWorldSpace(proj * World::camera.getView());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center - World::lightDirection, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 40.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}


std::vector<glm::mat4> getLightSpaceMatrices()
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(nearPlane, shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], farPlane));
        }
    }
    return ret;
}



