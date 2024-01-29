#version 330 core
out vec4 FragColor;

#define AMBIENT 0.5

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 camera;

uniform vec3 lightDirection;
uniform sampler2D shadowMap;

uniform int width;
uniform int height;

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);

uniform vec3 baseColor = vec3(1.0, 0.0, 1.0);

struct Texture {
    sampler2D id;
    vec3 offset;
    vec3 scale;
    float bumpMultiplier;
};

uniform Texture baseTexture;
uniform Texture bumpTexture;

void main() {
    vec2 baseTransformCoords = baseTexture.scale.xy * (TexCoords + baseTexture.offset.xy);
    vec2 bumpTransformCoords = bumpTexture.scale.xy * (TexCoords + bumpTexture.offset.xy);

    if (texture(baseTexture.id, baseTransformCoords).a < 0.1) {
        discard;
    }

    vec3 ambient = AMBIENT * lightColor;

    vec3 bitangent = cross(Normal, Tangent);
    mat3 tbn = mat3(Tangent, bitangent, Normal);

    vec3 texNormal = texture(bumpTexture.id, bumpTransformCoords).xyz;
    texNormal = texNormal * 2.0 - 1.0;
    vec3 normal = tbn * (texNormal * bumpTexture.bumpMultiplier);

    float specularStrength = 1.0;
    vec3 viewDir = normalize(camera - FragPos);
    vec3 reflectDir = reflect(lightDirection, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    float diffuse = max(dot(normal, -lightDirection), 0.0);

    vec4 res = vec4(ambient + diffuse + specular, 1.0) * texture(baseTexture.id, baseTransformCoords);

    FragColor = res;
}