
#include "model.h"

using namespace std;

struct FaceVertex {
    int vIndex;
    int tIndex;
    int nIndex;
};

void Model::loadModel(string path) {
    directory = path.substr(0, path.find_last_of('/')) + "/";

    ifstream infile(path);
    if (!infile) {
        cout << "ERROR::MODEL Couldn't find " << path << endl;
        return;
    }

    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> colors;

    vector<Vertex> vertices;
    vector<int> indices;

    map<string, Material> materials;
    Material currentMaterial;

    int vOffset = 0;
    int tOffset = 0;
    int nOffset = 0;

    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "o") {
            if (!vertices.empty()) {
                Mesh newMesh(vertices, indices);
                newMesh.material = currentMaterial;
                meshes.push_back(newMesh);

                vOffset = positions.size();
                tOffset = texCoords.size();
                nOffset = normals.size();

                positions.clear();
                normals.clear();
                texCoords.clear();
                colors.clear();
                vertices.clear();
                indices.clear();
            }
        }

        else if (type == "mtllib") {
            string materialPath;
            iss >> materialPath;

            vector<Material> newMaterials = loadMaterials(materialPath);

            for (auto mat : newMaterials) {
                materials.insert({mat.name, mat});
            }
        }
        else if (type == "usemtl") {
            string materialName;
            iss >> materialName;

            if (!vertices.empty()) {
                Mesh newMesh(vertices, indices);
                newMesh.material = currentMaterial;
                meshes.push_back(newMesh);

                vertices.clear();
                indices.clear();
            }

            currentMaterial = materials[materialName];
        }

        else if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            positions.emplace_back(x, y, z);
        }
        else if (type == "vn") {
            float x, y, z;
            iss >> x >> y >> z;
            normals.emplace_back(x, y, z);
        }
        else if (type == "vt") {
            float x, y;
            iss >> x >> y;
            texCoords.emplace_back(x, y);
        }

        else if (type == "f") {
            string section;
            vector<FaceVertex> faceData;

            while (iss >> section) {
                istringstream sss(section);
                string current;
                int n = 0;

                int vIndex;
                int tIndex;
                int nIndex;

                while (getline(sss, current, '/')) {
                    int index = stoi(current);
                    switch (n) {
                        case 0:
                            vIndex = (index - 1) - vOffset;
                            break;
                        case 1:
                            tIndex = (index - 1) - tOffset;
                            break;
                        case 2:
                            nIndex = (index - 1) - nOffset;
                            break;
                    }
                    n++;
                }

                faceData.push_back(FaceVertex{vIndex, tIndex, nIndex});
            }

            if (faceData.size() == 3) {
                for (int t = 0; t < 3; t++) {
                    Vertex newVertex(positions[faceData[t].vIndex],
                                     normals[faceData[t].nIndex],
                                     texCoords[faceData[t].tIndex]);

                    vertices.push_back(newVertex);

                    indices.push_back(vertices.size() - 1);

//                    indices.push_back(faceData[t].vIndex);
//                    vertices[indices.back()].texCoord = texCoords[faceData[t].tIndex];
//                    vertices[indices.back()].normal = normals[faceData[t].nIndex];
                }
            }
            else {
                // I have cheated and I am not sorry at all

                // Later edit: The OBJ file format is very weird and annoying

                glm::vec3 averagePosition = glm::vec3(0.0);
                glm::vec3 averageNormal = glm::vec3(0.0);
                glm::vec2 averageTexCoord = glm::vec3(0.0);

                for (int t = 0; t < faceData.size(); t++) {
                    averagePosition += positions[faceData[t].vIndex] * 0.25f;
                    averageNormal += normals[faceData[t].nIndex] * 0.25f;
                    averageTexCoord += texCoords[faceData[t].tIndex] * 0.25f;
                }

                Vertex center(averagePosition, averageNormal, averageTexCoord);
                vertices.push_back(center);
                int centerPosition = vertices.size() - 1;

                for (int t = 0; t < faceData.size(); t++) {
                    int next = (t + 1) % faceData.size();

                    Vertex newVertex(positions[faceData[t].vIndex],
                                     normals[faceData[t].nIndex],
                                     texCoords[faceData[t].tIndex]);

                    vertices.push_back(newVertex);

                    indices.push_back(vertices.size() - 1);

//                    indices.push_back(faceData[t].vIndex);
//                    vertices[indices.back()].texCoord = texCoords[faceData[t].tIndex];
//                    vertices[indices.back()].normal = normals[faceData[t].nIndex];

                    Vertex nextVertex(positions[faceData[next].vIndex],
                                     normals[faceData[next].nIndex],
                                     texCoords[faceData[next].tIndex]);

                    vertices.push_back(nextVertex);

                    indices.push_back(vertices.size() - 1);

//                    indices.push_back(faceData[next].vIndex);
//                    vertices[indices.back()].texCoord = texCoords[faceData[next].tIndex];
//                    vertices[indices.back()].normal = normals[faceData[next].nIndex];

                    indices.push_back(centerPosition);

//                    indices.push_back(vertices.size() - 1);
                }
            }
        }
    }

    if (!vertices.empty()) {
        Mesh newMesh(vertices, indices);
        newMesh.material = currentMaterial;
        meshes.push_back(newMesh);
    }

    infile.close();
}

vector<Material> Model::loadMaterials(string file) {
    vector<Material> materials;

    ifstream infile(directory + file);
    if (!infile) {
        cout << "ERROR::MATERIAL Couldn't find " << directory + file << endl;
        return vector<Material>();
    }

    Material mat;

    string line;

    while (getline(infile, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "Kd") {
            float x, y, z;
            iss >> x >> y >> z;
            mat.baseColor = glm::vec3(x, y, z);
        }
        else if (type == "Ka") {
            float x, y, z;
            iss >> x >> y >> z;
            mat.specularColor = glm::vec3(x, y, z);
        }
        else if (type == "Ke") {
            float x, y, z;
            iss >> x >> y >> z;
            mat.emissiveColor = glm::vec3(x, y, z);
        }

        else if (type == "map_Kd") {
            string textureName;
            iss >> textureName;

            Texture2D baseTexture((directory + textureName).c_str());
            mat.baseTexture = baseTexture;
        }

        else if (type == "newmtl") {
            if (!mat.name.empty()) {
                materials.push_back(mat);
                mat = Material{};
            }
            iss >> mat.name;
        }
    }

    infile.close();

    materials.push_back(mat);

    return materials;
}

void Model::draw(ShaderProgram &shader) {
    for (auto mesh : meshes) {
        mesh.draw(shader);
    }
}

