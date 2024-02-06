#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform Texture color;
uniform Texture depth;

void main()
{
    vec3 col = texture(color.id, TexCoords).xyz;

    FragColor = vec4(col, 1.0);
}