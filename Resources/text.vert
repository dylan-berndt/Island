
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoords;

mat4 ortho(float left, float right, float bottom, float top, float n, float f) {
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(f + n) / (f - n);

    return mat4(
    2.0 / (right - left), 0.0, 0.0, 0.0,
    0.0, 2.0 / (top - bottom), 0.0, 0.0,
    0.0, 0.0, -2.0 / (f - n), 0.0,
    tx, ty, tz, 1.0
    );
}

uniform mat4 textProjection;

void main()
{
//    gl_Position = ortho(0, width, 0, height, near, far) * vec4(aPos.xy, 0.0, 1.0);
    gl_Position = textProjection * vec4(aPos.xy + vec2(width, height) * 0.5, 0.0, 1.0);
    TexCoords = texCoord.xy;
}