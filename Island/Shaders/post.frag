#version 330 core

#define PI 3.14159

in vec2 TexCoord;

out vec4 FragColor;

struct Texture {
    sampler2D id;
    vec3 offset;
    vec3 scale;
    float bumpMultiplier;
};

uniform Texture color;
uniform Texture depth;

uniform sampler2DArray shadowMap;
layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

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

ivec2 colorSize = textureSize(color.id, 0);

vec2 colorTexelSize = 1.0 / vec2(colorSize);

float near = 0.1;
float far = 500.0;

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

int getShadowLayer(vec3 pos) {
    vec4 fragPosViewSpace = view * vec4(pos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }
    return layer;
}

vec3 shadow(vec3 original, vec3 pos) {
    int layer = getShadowLayer(pos);

    vec4 position = lightSpaceMatrices[layer] * vec4(pos, 1.0);
    vec3 projCoords = position.xyz / position.w;

    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    if (currentDepth > 1.0) {
        return original;
    }

    float bias = 0.0001;
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (far * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));

    int shadowKernel = 5;
    for(int x = -shadowKernel / 2; x <= shadowKernel / 2; ++x)
    {
        for(int y = -shadowKernel / 2; y <= shadowKernel / 2; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= float(shadowKernel * shadowKernel);

    if(projCoords.z > 1.0 || projCoords.z < 0.0) {
        shadow = 0.0;
    }

    shadow = clamp(shadow, 0.0, 0.6);

    return (1.0 - shadow) * original;
}

void main() {
    vec3 original = texture(color.id, TexCoord).xyz;
    vec3 pos = worldPosition(TexCoord, texture(depth.id, TexCoord).x);

//    vec3 depthOfField = field(original, TexCoord, pos);

    vec3 shadowed = shadow(original, pos);

    vec3 bloomed = bloom(shadowed, TexCoord, color);

    vec3 fogged = fog(bloomed, pos);

//    int layer = getShadowLayer(pos);
//
//    vec4 position = lightSpaceMatrices[layer] * vec4(pos, 1.0);
//    vec3 projCoords = position.xyz / position.w;
//
//    projCoords = projCoords * 0.5 + 0.5;

//    if (projCoords.x > 0 && projCoords.x < 1.0 && projCoords.y > 0 && projCoords.y < 1.0 && projCoords.z > 0 && projCoords.z < 1.0) {
//        FragColor = vec4(projCoords.xy, 0.0, 1.0);
//        FragColor = vec4(vec3(texture(shadowMap, vec3(projCoords.xy, layer)).r), 1.0);
//    }
//    else{
//        FragColor = vec4(shadowed, 1.0);
//    }
//    FragColor = vec4(projCoords.xy, 0.0, 1.0);
//    FragColor = vec4(float(layer) / float(cascadeCount), 0.0, 0.0, 1.0);
//    FragColor = vec4(vec3(texture(shadowMap, vec3(projCoords.xy, layer)).r), 1.0);
//    FragColor = vec4(fogged, 1.0);

    vec3 color = fogged;
//    vec3 color = pow(shadowed, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);

//    float depthValue = texture(shadowMap, vec3(TexCoord, 0)).r;
//    FragColor = vec4(vec3(depthValue), 1.0);

//    FragColor = texture(color.id, TexCoord);
}

