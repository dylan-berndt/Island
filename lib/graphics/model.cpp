
#include "model.h"

using namespace std;

struct FaceVertex {
    int vIndex;
    int tIndex;
    int nIndex;
};

Texture2D loadTexture(string directory, string line) {
    istringstream iss(line);
    string parse;

    glm::vec3 scale = glm::vec3(1.0);
    glm::vec3 offset = glm::vec3(0.0);
    float bumpMultiplier = 1.0;
    string name;

    iss >> parse;

    while (iss >> parse) {
        if (parse == "-bm") {
            iss >> bumpMultiplier;
        }
        else if (parse == "-s") {
            iss >> scale.x >> scale.y >> scale.z;
        }
        else if (parse == "-o") {
            iss >> offset.x >> offset.y >> offset.z;
        }
        else if (parse[0] != '-') {
            getline(iss, name);
            name = parse + name;
        }
    }

//    cout << directory + name << endl;
//    cout << scale.x << " " << scale.y << " " << scale.z << endl;
//    cout << offset.x << " " << offset.y << " " << offset.z << endl << endl;

    Texture2D texture((directory + name).c_str());
    texture.scale = scale; texture.offset = offset; texture.bumpMultiplier = bumpMultiplier;
    return texture;
}

void Model::loadModel(string path) {
    directory = path.substr(0, path.find_last_of('/')) + "/";

    ifstream infile(path);
    if (!infile) {
        Log << "\aERROR::MODEL Couldn't find " << path << "\a" << endl;
        return;
    }

    #ifdef DEBUG
        Log << "Loaded model " << path << endl;
    #endif

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

                vOffset += positions.size();
                tOffset += texCoords.size();
                nOffset += normals.size();

                positions.clear();
                normals.clear();
                texCoords.clear();
                colors.clear();
                vertices.clear();
                indices.clear();
            }
        }

        else if (type == "mtllib") {
            int space = line.find_first_of(' ');
            string materialPath = line.substr(space + 1, line.length() - space + 1);

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
                }
            }
            else {
                // I have cheated and I am not sorry at all

                // Later edit: The OBJ file format is very weird and annoying

                glm::vec3 averagePosition = glm::vec3(0.0);
                glm::vec3 averageNormal = glm::vec3(0.0);
                glm::vec2 averageTexCoord = glm::vec3(0.0);

                for (int t = 0; t < faceData.size(); t++) {
                    averagePosition += positions[faceData[t].vIndex] * (1.0f / faceData.size());
                    averageNormal += normals[faceData[t].nIndex] * (1.0f / faceData.size());
                    averageTexCoord += texCoords[faceData[t].tIndex] * (1.0f / faceData.size());
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

                    Vertex nextVertex(positions[faceData[next].vIndex],
                                     normals[faceData[next].nIndex],
                                     texCoords[faceData[next].tIndex]);

                    vertices.push_back(nextVertex);

                    indices.push_back(vertices.size() - 1);

                    indices.push_back(centerPosition);
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

vector<Material> Model::loadMaterials(string path) {
    vector<Material> materials;

    #ifdef DEBUG
        Log << "Loaded material " << path << endl;
    #endif

    ifstream infile(directory + path);
    if (!infile) {
        Log << "\aERROR::MATERIAL Couldn't find " << directory + path << "\a" << endl;
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
            mat.assign("baseColor", glm::vec3(x, y, z));
        }
        else if (type == "Ka") {
            float x, y, z;
            iss >> x >> y >> z;
            mat.assign("specularColor", glm::vec3(x, y, z));
        }
        else if (type == "Ke") {
            float x, y, z;
            iss >> x >> y >> z;
            mat.assign("emissiveColor", glm::vec3(x, y, z));
        }

        else if (type == "map_Kd") {
            mat.assign("baseTexture", loadTexture(directory, line));
        }

        else if (type == "map_Bump" || type == "map_bump") {
            mat.assign("bumpTexture", loadTexture(directory, line));
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

