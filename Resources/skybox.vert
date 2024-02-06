#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aTangent;

out vec3 TexCoords;

void main() {
    TexCoords = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}