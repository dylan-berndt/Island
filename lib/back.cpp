#include "external/glad/glad.h"
#include "iostream"


float* flatMesh(float width, float depth, int dx, int dz, float height) {
    float* mesh = new float[dx * dz * 3];

    for (int pz = 0; pz < dz; pz++) {
        for (int px = 0; px < dx; px++) {
            float x = width * (px / (dx / 2) - 1.0);
            float y = height;
            float z = depth * (pz / (dz / 2) - 1.0);

            int start = pz * dx + px;
            mesh[start] = x;
            mesh[start + 1] = y;
            mesh[start + 2] = z;
        }
    }

    return mesh;
}

unsigned int bindVertices(float* vertices, int mode) {
    unsigned int VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, mode);

    return VBO;
}


