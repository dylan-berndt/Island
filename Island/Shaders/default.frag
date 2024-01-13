#version 330 core
out vec4 FragColor;

#define AMBIENT 0.5
#define LIGHT_DIRECTION normalize(vec3(1.0, -1.0, 1.0))

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

in vec4 ShadowPos;

uniform mat4 model;
uniform mat4 view;
uniform vec3 camera;

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);

uniform mat4 lightSpace;
uniform sampler2D shadowMap;

uniform vec3 baseColor = vec3(1.0, 0.0, 1.0);

uniform sampler2D baseTexture;

float shadowCalc(vec4 position) {
    vec3 projCoords = position.xyz / position.w;

    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    float bias = 0.0001;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;

    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main() {
    vec3 ambient = AMBIENT * lightColor;

    vec3 lightDir = LIGHT_DIRECTION;

    float specularStrength = 1.0;
    vec3 viewDir = normalize(camera - FragPos);
    vec3 reflectDir = reflect(lightDir, Normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    float diffuse = max(dot(Normal, -lightDir), 0.0);

//    float shadow = shadowCalc(ShadowPos);
    float shadow = 0.0;
    vec4 res = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0) * texture(baseTexture, TexCoords);

    FragColor = res;
}