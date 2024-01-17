#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 lightSpace;
uniform mat4 model;

void main() {
    gl_Position = lightSpace * model * vec4(aPos, 1.0);
}