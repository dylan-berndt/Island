#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;

void main() {
    TexCoord = texCoord;

    gl_Position = vec4(aPos, 1.0);
}
