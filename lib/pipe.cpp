
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
Font *Window::defaultFont;
function<void(GLFWwindow*, int, int, int, int)> Window::userKeyCallback;
function<void(GLFWwindow*)> Window::userKeyPress;
function<void(string)> Window::userCommand;

Camera World::camera;
glm::vec3 World::lightDirection;
glm::vec3 World::lightColor;
SkyBox *World::skybox = nullptr;

int shadowMapResolution = 2048;
vector<float> ShaderProgram::shadowCascadeLevels = {50.0f, 25.0f, 15.0f, 5.0f, 2.0f};
int ShaderProgram::shadowMap;

float previousFrameTime = 0.0;

bool minimised = false;
bool console = false;
string currentCommand;
int scrollOffset = 0;
bool wireframe = false;

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

void loadScene(string path) {
    ifstream scene(path);

    if (!scene.good()) {
        Log << "\aERROR::SCENE Couldn't load " << path << "\a" << endl;
        return;
    }

    Log.str("");
    Commands.str("");

    Entity::entities = vector<Entity *>();
    Entity::entityMap.clear();

    Log << "Loading scene " << path << endl;

    string line;
    while (getline(scene, line)) {
        glfwPollEvents();
        glViewport(0, 0, Window::width, Window::height);
        glClearColor(0.1f, 0.125f, 0.125f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        string output = Log.str();
        Window::defaultFont->render(output, 20, Window::height - 40, glm::vec3(1.0), glm::vec3(0.8));

        if (glfwWindowShouldClose(Window::self)) {
            glfwDestroyWindow(Window::self);
            glfwTerminate();
            exit(0);
        }

        glfwSwapBuffers(Window::self);

        command(line);
    }

    scene.close();
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
            GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,  shadowMapResolution, shadowMapResolution, ShaderProgram::shadowCascadeLevels.size() + 1,
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

    ShaderProgram::shadowMap = shadowTexture->id();

    screen->material.assign("color", *colorTexture);
    screen->material.assign("depth", *depthTexture);

    ShaderProgram::width = width;
    ShaderProgram::height = height;

    shadowBuffer->unbind();
}

void initialize() {
    ifstream config("config.txt");

    config >> Window::width >> Window::height >> Window::name;

    config >> File::resourceLocation;

    string sceneName;
    config >> sceneName;

    int flag;
    for (int i = 0; i < 3; i++) {
        config >> flag;
        severityFlags.push_back(flag);
    }

    if (!glfwInit()) {
        Log << "\aCan't Initialize GLFW\a" << endl;
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

    ShaderProgram::defaultShader = new ShaderProgram(File::getPath("Resources/default"));
    ShaderProgram::shadowShader = new ShaderProgram(File::getPath("Resources/shadow"));
    ShaderProgram::skyboxShader = new ShaderProgram(File::getPath("Resources/skybox"));
    ShaderProgram::postShader = new ShaderProgram(File::getPath("Resources/defaultPost"));
    ShaderProgram::textShader = new ShaderProgram(File::getPath("Resources/text"));

    Window::initializePostProcessing();

    Window::defaultFont = new Font("C:/Windows/Fonts/consola.ttf", 15);

    glfwSetKeyCallback(Window::self, Window::keyCallback);
    glfwSetCharCallback(Window::self, Window::charCallback);
    glfwSetFramebufferSizeCallback(Window::self, Window::frameBufferSizeCallback);
    glfwSetScrollCallback(Window::self, Window::mouseScrollCallback);

    loadScene(File::getPath(sceneName));
}

void update(float &delta) {
    delta = glfwGetTime() - previousFrameTime;
    previousFrameTime = glfwGetTime();

    Entity *camera = Entity::entityMap["Camera"];
    Transform *cameraTransform = camera->transform();

    ShaderProgram::camera = cameraTransform->position;
    ShaderProgram::view = World::camera.getView();
    ShaderProgram::projection = World::camera.getProjection();
    ShaderProgram::lightDirection = World::lightDirection;
    ShaderProgram::lightColor = World::lightColor;
    ShaderProgram::nearPlane = World::camera.nearPlane;
    ShaderProgram::farPlane = World::camera.farPlane;

    for (auto entity : Entity::entities) {
        entity->update(delta);
        if (glError()) {
            Log << "\aWhile " << entity->name << " updating\a" << endl;
        }
    }

    glfwPollEvents();

    Window::keyPress(Window::self);
}

template <class T>
int performAssign(T& instance, string assignName, string value) {
    int success;

    value = value.substr(value.find_first_not_of(' '));

    if (value == "true" || value == "false") {
        success = instance.assign(assignName, value == "true");
    }
    else if (isdigit(value.at(0)) || value[0] == '-') {
        if (value.find(' ') == value.npos) {
            if (value.find('.') != value.npos || value.find('f') != value.npos) {
                float f = stof(value);
                success = instance.assign(assignName, f);
            }
            else {
                int i = stoi(value);
                success = instance.assign(assignName, i);
            }
        }
        else {
            int n;
            float num;
            vector<float> nums;
            istringstream vec(value);
            while (vec >> num) {
                nums.push_back(num);
                n++;
            }
            if (n == 2) {
                success = instance.assign(assignName, glm::vec2(nums[0], nums[1]));
            }
            if (n == 3) {
                success = instance.assign(assignName, glm::vec3(nums[0], nums[1], nums[2]));
            }
            if (n == 4) {
                success = instance.assign(assignName, glm::vec4(nums[0], nums[1], nums[2], nums[3]));
            }
        }
    }
    else {
        success = instance.assign(assignName, value);
    }

    return success;
}

void command(string input) {
    istringstream iss(input);

    string block;
    iss >> block;

    if (block == "GLOBAL_LIGHT_DIRECTION") {
        float x, y, z;
        if (!(iss >> x >> y >> z)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
        }
        World::lightDirection = glm::normalize(glm::vec3(x, y, z));
    }
    else if (block == "GLOBAL_LIGHT_COLOR") {
        float x, y, z;
        if (!(iss >> x >> y >> z)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
        }
        World::lightColor = glm::vec3(x, y, z);
    }
    else if (block == "CAMERA") {
        float x, y, z, rx, ry, rz;
        if (!(iss >> x >> y >> z >> rx >> ry >> rz)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
            return;
        }
        float fov, cameraNear, cameraFar;
        if (!(iss >> fov >> cameraNear >> cameraFar)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
            return;
        }
        World::camera = Camera();
        World::camera.fov = fov; World::camera.nearPlane = cameraNear; World::camera.farPlane = cameraFar;
        World::camera.aspect = float(Window::width) / float(Window::height);

        if (!Entity::entityMap.count("Camera")) {
            Transform *cameraTransform = new Transform(glm::vec3(x, y, z), glm::vec3(1.0), glm::vec3(rx, ry, rz));
            Entity *worldCamera = new Entity("Camera", cameraTransform);
            worldCamera->addComponent("Camera", &World::camera);
        }
    }
    else if (block == "FOV") {
        float f;
        if (!(iss >> f)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
            return;
        }
        else {
            World::camera.fov = f;
        }
    }
    else if (block == "SKYBOX") {
        string path;
        iss >> path;

        World::skybox = new SkyBox(File::getPath(path));
    }
    else if (block == "MODEL") {
        string name;
        if (!(iss >> name)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
            return;
        }

        string path;
        string section;

        if (!(iss >> section)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
            return;
        }

        bool cull = true;

        while (section[0] == '-') {
            if (section[1] == 'C') {
                cull = false;
            }
            iss >> section;
        }

        path += section;
        while (iss >> section && section[0] != '(') {
            path += " " + section;
        }

        ModelComponent *modelComponent = new ModelComponent(File::getPath(path));
        modelComponent->cullFace = cull;

        string transformationData;
        getline(iss,  transformationData);

        Transform *transform = transformFromString(section + transformationData);

        Entity *newModel = new Entity(name, transform);
        newModel->addComponent("ModelComponent", modelComponent);
    }
    else if (block == "SET") {
        string name;
        iss >> name;
        if (!Entity::entityMap.count(name)) {
            Log << "\aINVALID COMMAND: " << name << " does not exist\a" << endl;
            return;
        }
        Entity *edit = Entity::entityMap[name];

        string componentType;
        iss >> componentType;

        string assignName;
        iss >> assignName;

        string value;
        getline(iss, value);

        Component *component = edit->getComponent(componentType);

        if (component == nullptr) {
            Log << "\aINVALID COMMAND: Component " << componentType << " not on " << name << endl;
            return;
        }
        int success = performAssign(*component, assignName, value);

        if (success == 1) {
            Log << name << "." << componentType << "." << assignName << " = " << value << endl;
        }
        else {
            Log << "\aINVALID COMMAND " << input << "\a" << endl;
            return;
        }
    }
    else if (block == "UNIFORM") {
        string name;
        iss >> name;

        string uniformName;
        iss >> uniformName;

        string value;
        getline(iss, value);

        ShaderProgram shader = ShaderProgram::shaders[name];

        shader.use();

        int success = performAssign(shader, uniformName, value);

        int error = glError();

        shader.stop();

        if (success == 1 && !error) {
            Log << name << "." << uniformName << " = " << value << endl;
        }
        else {
            Log << "\aINVALID COMMAND " << input << "\a" << endl;
            return;
        }
    }
    else if (block == "MATERIAL") {
        string name;
        iss >> name;

        if (!Entity::entityMap.count(name)) {
            Log << "\aINVALID COMMAND: " << name << " does not exist\a" << endl;
            return;
        }
        Entity *edit = Entity::entityMap[name];

        string valueName;
        iss >> valueName;

        string value;
        iss >> value;

        MeshComponent<Mesh> *mesh = edit->getComponent<MeshComponent<Mesh>>();
        Material mat = mesh->getMaterial();

        performAssign(mat, valueName, value);
    }
    else if (block == "SHADERS") {
        Log << endl << "Current Shaders: " << endl;
        for (auto shader : ShaderProgram::shaders) {
            Log << shader.second.name << endl;
        }
    }
    else if (block == "ENTITY") {
        string name;
        iss >> name;

        string edit;
        iss >> edit;

        string value;
        iss >> value;

        if (!Entity::entityMap.count(name) && edit != "CREATE") {
            Log << "\aINVALID COMMAND: " << name << " does not exist\a" << endl;
            return;
        }

        if (edit == "NAME") {
            Entity::entityMap[name]->name = value;
        }
        else if (edit == "ACTIVE") {
            Entity::entityMap[name]->active = value == "true";
        }
        else if (edit == "CREATE") {
            Entity *newEntity = new Entity(name, transformFromString(value));
        }
        else if (edit == "DELETE") {
            Entity* entity = Entity::entityMap[name];
            auto it = find(Entity::entities.begin(), Entity::entities.end(), entity);
            Entity::entities.erase(it);
            Entity::entityMap.erase(name);
        }
        else {
            Log << "\aINVALID COMMAND\a" << endl;
            return;
        }
    }
    else if (block == "ENTITIES") {
        Log << endl << "Current Entities: " << endl;
        for (auto entity : Entity::entityMap) {
            Log << entity.second->name << endl;
            for (auto componentName : entity.second->componentNames) {
                Log << "    " << componentName << endl;
            }
        }
    }
    else if (block == "WIREFRAME") {
        wireframe = !wireframe;
    }
    else if (block == "LOAD") {
        string sceneName;
        if (!(iss >> sceneName)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
            return;
        }

        loadScene(File::getPath(sceneName));
        return;
    }
    else if (block == "SAVE") {
        string fileName;
        if (!(iss >> fileName)) {
            Log << "\aINVALID COMMAND: " << input << "\a" << endl;
            return;
        }

        ofstream file(File::getPath(fileName));

        if (!file) {

        }
        else {
            file << Commands.rdbuf();

            Log << "Saved to " << fileName << endl;
        }

        file.close();
        return;
    }
    else {
        if (Window::userCommand != nullptr) {
            Window::userCommand(iss.str());
        }
    }

    Commands << input << endl;
}

void Window::draw() {
    if (minimised) {
        return;
    }

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

    for (auto entity : Entity::entities) {
        entity->draw(*ShaderProgram::shadowShader);
    }

    ShaderProgram::shadowShader->stop();

    glCullFace(GL_BACK);
    shadowBuffer->unbind();

    glViewport(0, 0, Window::width, Window::height);

    postProcessingBuffer->bind();

    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (World::skybox != nullptr) {
        World::skybox->draw(*ShaderProgram::skyboxShader);
    }

    for (auto entity : Entity::entities) {
        entity->draw();
        if (glError()) {
            Log << "\aWhile " << entity->name << " drawing\a" << endl;
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    postProcessingBuffer->unbind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderProgram::postShader->use();

    ShaderProgram::postShader->assign("cascadeCount", (int) ShaderProgram::shadowCascadeLevels.size());
    for (size_t i = 0; i < ShaderProgram::shadowCascadeLevels.size(); ++i)
    {
        ShaderProgram::postShader->assign("cascadePlaneDistances[" + std::to_string(i) + "]",
                                          World::camera.farPlane / ShaderProgram::shadowCascadeLevels[i]);
    }

    blit();

    ShaderProgram::postShader->stop();

    if (console) {
        glEnable(GL_SCISSOR_TEST);
        glClearColor(0.1f, 0.125f, 0.125f, 1.0f);

        glScissor(15, height - 35, 200, 30);
        glClear(GL_COLOR_BUFFER_BIT);
        defaultFont->render("Console: ", 25, height - 25, glm::vec3(1.0), glm::vec3(1.0));

        glScissor(15, 80, 800, height - 140);
        glClear(GL_COLOR_BUFFER_BIT);
        defaultFont->render(Log.str(), 25, height - 80 + scrollOffset, glm::vec3(1.0), glm::vec3(1.0),
                            *ShaderProgram::textShader, 600);

        glScissor(15, 25, 800, 30);
        glClear(GL_COLOR_BUFFER_BIT);
        defaultFont->render(currentCommand, 25, 35, glm::vec3(1.0), glm::vec3(1.0));
        glDisable(GL_SCISSOR_TEST);
    }

    glfwSwapBuffers(self);
}

void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (userKeyCallback != nullptr && !console) {
        userKeyCallback(window, key, scancode, action, mods);
    }

    if (action == GLFW_PRESS) {
        if (mods & GLFW_MOD_CONTROL) {
            if (key == GLFW_KEY_SLASH) {
                console = !console;
                if (!console) {
                    currentCommand = "";
                }
            }
        }

        if (console) {
            if (key == GLFW_KEY_BACKSPACE) {
                if (currentCommand.length() > 0) {
                    currentCommand.pop_back();
                }
            }

            if (key == GLFW_KEY_ENTER) {
                command(currentCommand);
                currentCommand = "";
            }
        }
    }
}

void Window::charCallback(GLFWwindow *window, unsigned int codepoint) {
    if (console) {
        currentCommand += char(codepoint);
    }
}

void Window::keyPress(GLFWwindow *window) {
    if (userKeyPress != nullptr && !console) {
        userKeyPress(window);
    }
}

void Window::mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    scrollOffset -= 20 * yOffset;
}

void Window::frameBufferSizeCallback(GLFWwindow *win, int w, int h) {
    minimised = w == 0 || h == 0;

    colorTexture->create(w, h, GL_RGB);
    depthTexture->create(w, h, GL_DEPTH_COMPONENT, nullptr, GL_FLOAT);
    width = w;
    height = h;

    ShaderProgram::width = w;
    ShaderProgram::height = h;
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
            glm::radians(World::camera.fov), World::camera.aspect, localNear, localFar);
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
    for (size_t i = 0; i < ShaderProgram::shadowCascadeLevels.size() + 1; ++i)
    {
        float cascadeLevel = World::camera.farPlane / ShaderProgram::shadowCascadeLevels[i];
        float previousCascade = World::camera.farPlane / ShaderProgram::shadowCascadeLevels[i - 1];
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(World::camera.nearPlane, cascadeLevel));
        }
        else if (i < ShaderProgram::shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(previousCascade, cascadeLevel));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(previousCascade, World::camera.farPlane));
        }
    }
    return ret;
}



