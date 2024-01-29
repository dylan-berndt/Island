
#include "imports.h"

std::ostream& operator<< (std::ostream& os, glm::vec1 v) {
    os << v.x;
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::vec2 v) {
    os << v.x << ", " << v.y;
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::vec3 v) {
    os << v.x << ", " << v.y << ", " << v.z;
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::mat3 v) {
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            os << v[y][x] << " ";
        }
        os << std::endl;
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, glm::mat4 v) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            os << v[y][x] << " ";
        }
        os << std::endl;
    }
    return os;
}

