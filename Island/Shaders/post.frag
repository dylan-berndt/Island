#version 330 core

#define PI 3.14159

in vec2 TexCoord;

out vec4 FragColor;

struct Texture {
    sampler2D id;
    vec3 offset;
    vec3 scale;
};

uniform Texture color;
uniform Texture depth;

uniform Texture shadowMap;
uniform mat4 lightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 camera;

#define LOW_FOG vec3(0.6, 0.9, 1.0)
#define HIGH_FOG vec3(1.6, 1.6, 1.6)

float fogDensity = 1.5;
float fogDistance = 400.0;

float bloomEffect = 0.5;
uniform int bloomKernelSize = 4;
float bloomKernelSizeMultiplier = 4.0;

uniform int depthOfFieldKernelSize = 4;
float depthOfFieldKernelSizeMultiplier = 4.0;
uniform float depthOfFieldDistance = 10.0;
uniform float depthOfFieldEffect = 0.6;

ivec2 colorSize = textureSize(color.id, 0);

vec2 colorTexelSize = 1.0 / vec2(colorSize);

float near = 0.1;
float far = 80.0;

float linearDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float gaussian(int x, int y, int kernel) {
    float coefficient = 1.0 / (2.0 * PI * pow(kernel, 0.5));
    float exponent = -(x * x * y * y) / (2.0 * pow(kernel, 0.5));
    return coefficient * exp(exponent);
}

vec3 bloom(vec3 original, vec2 position, Texture sampler) {
    vec3 bloom_add = vec3(0.0);

    for (int x = -bloomKernelSize / 2; x < bloomKernelSize / 2; x++) {
        for (int y = -bloomKernelSize / 2; y < bloomKernelSize / 2; y++) {
            if (x == 0 && y == 0) {
                continue;
            }

            vec2 sample_position = bloomKernelSizeMultiplier * vec2(x, y) * colorTexelSize + position;
            vec3 sample_col = texture(sampler.id, sample_position).xyz;

            vec3 bloom_mult = clamp(sample_col - 0.6, 0.0, 1.0);

            float gauss = gaussian(x, y, bloomKernelSize);
            bloom_add += gauss * bloom_mult * sample_col * bloomKernelSizeMultiplier;
        }
    }
    return original + bloom_add * bloomEffect;
}

vec3 worldPosition(vec2 uv, float depth) {
    vec4 boxPos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 pos = inverse(projection * view) * boxPos;
    return pos.xyz / pos.w;
}

vec3 fog(vec3 original, vec3 pos) {
    float depthSpread = max(1.0 - (length(pos - camera) / fogDistance), 0.0);

    vec3 current_fog = mix(LOW_FOG, HIGH_FOG, max(normalize(pos - camera).y, 0.0));

    float heightMult = max(min(1.0 - pos.y / fogDistance, 1.0), 0.0);

    if (length(pos - camera) > fogDistance && 1.0 - pos.y / fogDistance > 0.98) {
        return current_fog;
    }

    float fogFactor = exp(-fogDensity * fogDensity * depthSpread * depthSpread);
    vec3 fogged = mix(original, current_fog, fogFactor * heightMult);

    return fogged;
}

vec3 shadow(vec3 original, vec3 pos) {
    vec4 position = lightSpace * vec4(pos, 1.0);
    vec3 projCoords = position.xyz / position.w;

    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap.id, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(shadowMap.id, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;

    if(projCoords.z > 1.0 || projCoords.z < 0.0) {
        shadow = 0.0;
    }

    shadow = clamp(shadow, 0.0, 0.6);

    return (1.0 - shadow) * original;
}

vec3 field(vec3 original, vec2 uv, vec3 pos) {
    vec3 end = vec3(0);

    float effect = abs(length(pos - camera) - depthOfFieldDistance) / 10.0;
    effect = clamp(effect * depthOfFieldEffect, 0.0, 1.0);

    for (int x = -depthOfFieldKernelSize / 2; x < depthOfFieldKernelSize / 2; x++) {
        for (int y = -depthOfFieldKernelSize / 2; y < depthOfFieldKernelSize / 2; y++) {
            vec2 sample_position = vec2(x, y) * depthOfFieldKernelSizeMultiplier * colorTexelSize + uv;
            float gauss = gaussian(x, y, depthOfFieldKernelSize);
            end += texture(color.id, sample_position).xyz * gauss;
        }
    }

    return mix(original, end, effect);
}

mat4 bayer4x4 = mat4(0, 32, 8, 40,
48, 16, 56, 24,
12, 44, 4, 36,
60, 28, 52, 5);

mat2 bayer2x2 = mat2(0, 2,
3, 1);

void main() {
    vec3 original = texture(color.id, TexCoord).xyz;
    vec3 pos = worldPosition(TexCoord, texture(depth.id, TexCoord).x);

//    vec3 depthOfField = field(original, TexCoord, pos);

    vec3 bloomed = bloom(original, TexCoord, color);

    vec3 fogged = fog(bloomed, pos);

    vec3 shadowed = shadow(fogged, pos);

//    vec4 position = lightSpace * vec4(pos, 1.0);
//    vec3 projCoords = position.xyz / position.w;
//
//    projCoords = projCoords * 0.5 + 0.5;

//    if (projCoords.x > 0 && projCoords.x < 1.0 && projCoords.y > 0 && projCoords.y < 1.0 && projCoords.z > 0 && projCoords.z < 1.0) {
//        FragColor = vec4(projCoords.xy, 0.0, 1.0);
//        FragColor = vec4(vec3(texture(shadowMap.id, projCoords.xy).r), 1.0);
//    }
//    else{
//        FragColor = vec4(shadowed, 1.0);
//    }
//    FragColor = vec4(vec3(texture(shadowMap.id, TexCoord).r), 1.0);
//    FragColor = vec4(fogged, 1.0);

    FragColor = vec4(shadowed, 1.0);
}

