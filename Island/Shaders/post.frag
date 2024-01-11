#version 330 core

#define PI 3.14159

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D color;
uniform sampler2D depth;

uniform mat4 projection;
uniform mat4 view;

float fogDensity = 1.5;

#define LOW_FOG vec3(0.9, 0.8, 0.6) * 1.4
#define HIGH_FOG vec3(0.6, 0.8, 1.0) * 1.4

float bloomEffect = 1.0;
uniform int kernelSize = 4;
float sizeMultiplier = 4.0;

ivec2 textureSize = textureSize(color, 0);

vec2 texelSize = 1.0 / vec2(textureSize);

float near = 0.1;
float far = 500.0;

float linearDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float gaussian(int x, int y) {
    float coefficient = 1.0 / (2.0 * PI * pow(kernelSize, 0.5));
    float exponent = -(x * x * y * y) / (2.0 * pow(kernelSize, 0.5));
    return coefficient * exp(exponent);
}

vec3 bloom(vec3 original, vec2 position, sampler2D sampler) {
    vec3 bloom_add = vec3(0.0);

    for (int x = -kernelSize / 2; x < kernelSize / 2; x++) {
        for (int y = -kernelSize / 2; y < kernelSize / 2; y++) {
            if (x == 0 && y == 0) {
                continue;
            }

            vec2 sample_position = sizeMultiplier * vec2(x, y) * texelSize + position;
            vec3 sample_col = texture(sampler, sample_position).xyz;

            float brightness = sample_col.x + sample_col.y + sample_col.z;

            float bloom_mult = max(0.0, min(1.0, brightness - 0.6));

            float gauss = gaussian(x, y);
            bloom_add += gauss * bloom_mult * sample_col;
        }
    }
    return original + bloom_add * bloomEffect;
}

vec3 worldPosition(vec2 uv, float depth) {
    vec4 boxPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 pos = inverse(projection * view) * boxPos;
    return pos.xyz / pos.w;
}

vec3 fog(float depthValue, vec3 original, vec3 dir) {
    float depthSpread = max(1.0 - (depthValue / 100.0), 0.0);

    vec3 current_fog = mix(LOW_FOG, HIGH_FOG, dir.y / 200.0);

    float heightMult = max(min(1.0 - dir.y / 200.0, 1.0), 0.0);

    float fogFactor = exp(-fogDensity * fogDensity * depthSpread * depthSpread);
    vec3 fogged = mix(original, current_fog, fogFactor * heightMult);

    return fogged;
}

void main() {
    vec3 original = texture(color, TexCoord).xyz;

    vec3 bloomed = bloom(original, TexCoord, color);

    float depthValue = linearDepth(texture(depth, TexCoord).x);
    vec3 dir = worldPosition(TexCoord, texture(depth, TexCoord).x);
    vec3 fogged = fog(depthValue, bloomed, dir);

    FragColor = vec4(fogged, 1.0);
}
