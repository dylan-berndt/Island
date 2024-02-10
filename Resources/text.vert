
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoords;
uniform mat4 textProjection;

void main()
{
    gl_Position = textProjection * vec4(aPos.xy, 0.0, 1.0);
    TexCoords = texCoord.xy;
}