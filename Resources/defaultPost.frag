#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

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

float near = 0.1f;
float far = 500.0f;

vec3 worldPosition(vec2 uv, float depth) {
    vec4 boxPos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 pos = inverse(projection * view) * boxPos;
    return pos.xyz / pos.w;
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

    float bias = 0.00005;
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

    int shadowKernel = 3;
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

void main()
{
    vec3 position = worldPosition(TexCoords, texture(depth.id, TexCoords).r);

    vec3 col = texture(color.id, TexCoords).xyz;
    FragColor = vec4(shadow(col, position), 1.0);
}