#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    float alpha = texture(text, TexCoords).r;

    if (alpha < 0.5) {
        gl_FragDepth = 1.0;
    }
    else {
        gl_FragDepth = 0.0;
    }

    vec4 sampled = vec4(1.0, 1.0, 1.0, alpha);
    FragColor = vec4(textColor, 1.0) * sampled;
}