#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D color;
uniform sampler2D depth;

#define MAX_DISTANCE 200.0

void main() {
    FragColor = texture(color, TexCoords);
}
