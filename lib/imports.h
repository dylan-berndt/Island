

#ifndef PIPE_BACK_H
#define PIPE_BACK_H

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

std::ostream& operator<< (std::ostream& os, glm::vec1 v);
std::ostream& operator<< (std::ostream& os, glm::vec2 v);
std::ostream& operator<< (std::ostream& os, glm::vec3 v);
std::ostream& operator<< (std::ostream& os, glm::vec4 v);
std::ostream& operator<< (std::ostream& os, glm::mat3 v);
std::ostream& operator<< (std::ostream& os, glm::mat4 v);

class File {
public:
    static std::string resourceLocation;
    static std::string getPath(std::string path);
};

int glError();

#endif //PIPE_BACK_H
