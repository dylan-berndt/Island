

#ifndef PIPE_OBJ_H
#define PIPE_OBJ_H

#include "../imports.h"
#include "mesh.h"

class Cube : public Mesh {
public:
    Cube();
};

class Square : public Mesh {
public:
    Square();
};

class SkyBox : public Cube {
public:
    SkyBox(std::vector<std::string> faces);
    SkyBox(CubeMap c) : Cube() {cubeMap = c;};
    void draw(ShaderProgram &shader);
private:
    CubeMap cubeMap;
};



#endif //PIPE_OBJ_H
