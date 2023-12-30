#version 330 core

#define PI 3.14159

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D color;
uniform sampler2D depth;

float fogDensity = 1.5;

#define HIGH_FOG vec3(1.0, 0.8, 0.3) * 1.4
#define LOW_FOG vec3(0.6, 0.8, 1.0) * 1.4

float bloom_effect = 1.5;
int kernel_size = 4;
float size_multiplier = 2.0;

ivec2 textureSize = textureSize(color, 0);

vec2 texelSize = 1.0 / vec2(textureSize);

float gaussian(int x, int y) {
    float coefficient = 1.0 / (2.0 * PI * pow(kernel_size, 0.5));
    float exponent = -(x * x * y * y) / (2.0 * pow(kernel_size, 0.5));
    return coefficient * exp(exponent);
}

vec3 bloom(vec3 original, vec2 position, sampler2D sampler) {
    vec3 bloom_add = vec3(0.0);

    for (int x = -kernel_size / 2; x < kernel_size / 2; x++) {
        for (int y = -kernel_size / 2; y < kernel_size / 2; y++) {
            if (x == 0 && y == 0) {
                continue;
            }

            vec2 sample_position = size_multiplier * vec2(x, y) * texelSize + position;
            vec3 sample_col = texture(sampler, sample_position).xyz;

            float brightness = sample_col.x + sample_col.y + sample_col.z;

            float bloom_mult = max(0.0, min(1.0, brightness - 0.6));

            float gauss = gaussian(x, y);
            bloom_add += gauss * bloom_mult * sample_col;
        }
    }
    return original + bloom_add * bloom_effect;
}

vec3 fog(float depthValue, vec3 original) {
    float spread = 200.0;
    float depthSpread = 1.0 - (depthValue - (1.0 - (1.0 / spread))) * spread;
    depthSpread = max(0.0, min(1.0, depthSpread));

    vec3 current_fog = mix(LOW_FOG, HIGH_FOG, TexCoord.y - 0.3);

    float fogFactor = exp(-fogDensity * fogDensity * depthSpread * depthSpread);
    vec3 fogged = mix(original, current_fog, fogFactor);

    return fogged;
}

void main() {
    vec3 original = texture(color, TexCoord).xyz;

    float depthValue = texture(depth, TexCoord).x;

    vec3 bloomed = bloom(original, TexCoord, color);

    vec3 fogged = fog(depthValue, bloomed);

    FragColor = vec4(fogged, 1.0);
}
