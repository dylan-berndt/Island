
#include "model.h"

using namespace std;

void Model::loadModel(string path) {
    directory = path;

    ifstream infile(path);
    if (!infile) {
        return;
    }

    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> colors;

    vector<Vertex> vertices;
    vector<int> indices;
    vector<Texture2D> textures;

    Material material;

    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "o") {
            if (!vertices.empty()) {
                meshes.emplace_back(vertices, indices, textures);

                positions.clear();
                normals.clear();
                texCoords.clear();
                colors.clear();
                vertices.clear();
                indices.clear();
                textures.clear();
            }
        }

        else if (type == "mtllib") {
            string materialPath;
            iss >> materialPath;

        }

        else if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            positions.emplace_back(x, y, z);
            vertices.emplace_back(x, y, z);
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
            while (iss >> section) {

                istringstream sss(section);
                string current;
                int n = 0;

                while (getline(sss, current, '/')) {
                    int index = stoi(current);
                    switch (n) {
                        case 0:
                            indices.push_back(index - 1);
                            break;
                        case 1:
                            vertices[indices.back()].texCoord = texCoords[index - 1];
                            break;
                        case 2:
                            vertices[indices.back()].normal = normals[index - 1];
                            break;
                    }
                    n++;
                }
            }
        }
    }

    if (!vertices.empty()) {
        Mesh created(vertices, indices, textures);
        created.model = glm::scale(glm::mat4(1.0), glm::vec3(0.25, 1.0, 0.25));
        meshes.emplace_back(created);
    }
}

Material Model::loadMaterial(string path) {
    ifstream infile(path);
    string line;
    while (infile >> line) {
        cout << line << endl;
    }
}

vector<Texture2D> Model::loadMaterialTextures(Material *mat, string typeName) {
//    vector<Texture2D> textures;
//    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//    {
//        aiString str;
//        mat->GetTexture(type, i, &str);
//        cout << str.C_Str() << endl;
//        Texture2D texture(str.C_Str());
//        texture.type = typeName;
//        texture.path = str.C_Str();
//        textures.push_back(texture);
//    }
//    return textures;
}

void Model::draw(ShaderProgram &shader) {
    for (auto mesh : meshes) {
        mesh.draw(shader);
    }
}

