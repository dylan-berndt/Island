#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D color;
uniform sampler2D depth;

float fogDensity = 1.35;

#define LOW_FOG vec3(1.0, 0.8, 0.3)
#define HIGH_FOG vec3(0.6, 0.8, 1.0)

void main() {
    vec3 original = texture(color, TexCoord).xyz;

    float depthValue = texture(depth, TexCoord).x;

    float spread = 400.0;
    float depthSpread = 1.0 - (depthValue - (1.0 - (1.0 / spread))) * spread;
    depthSpread = max(0.0, min(1.0, depthSpread));

    vec3 current_fog = mix(LOW_FOG, HIGH_FOG, TexCoord.y - 0.3);

    float fogFactor = exp(-fogDensity * fogDensity * depthSpread * depthSpread);
    vec3 finalColor = mix(original, current_fog, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
