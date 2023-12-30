
#include "pipe.h"
using namespace std;

Vertex *flatMesh(float width, float depth, int nx, int nz, float height) {
    auto *mesh = new Vertex[nx * nz];

    int start = 0;
    for (int pz = 0; pz < nz; pz++) {
        for (int px = 0; px < nx; px++) {
            float x = width * (float(px) / (float(nz) / 2.0f) - 1.0f);
            float y = height;
            float z = depth * (float(pz) / (float(nz) / 2.0f) - 1.0f);

            mesh[start] = Vertex(x, y, z);
            start += 1;
        }
    }

    return mesh;
}

IndexedArray genWater(float width, float depth, int nx, int nz, float height) {
    Vertex *mesh = flatMesh(width, depth, nx, nz, height);

    unsigned int indices[6 * (nx - 1) * (nz - 1)];
    int in = 0;
    for (int z = 0; z < nz - 1; z++) {
        for (int x = 0; x < nx - 1; x++) {
            int tl = z * nx + x;
            int tr = tl + 1;
            int bl = (z + 1) * nx + x;
            int br = bl + 1;
            indices[in++] = tl; indices[in++] = bl; indices[in++] = tr;
            indices[in++] = tr; indices[in++] = bl; indices[in++] = br;
        }
    }

    IndexedArray v(mesh, nx * nz, indices, 6 * (nx - 1) * (nz - 1));

    return v;
}



