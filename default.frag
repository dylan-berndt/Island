#version 330 core
out vec4 FragColor;

#define AMBIENT 0.0
#define LIGHT_DIRECTION normalize(vec3(1.0, -0.1, 1.0))

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 camera;

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);

uniform vec3 baseColor = vec3(1.0, 0.0, 1.0);

uniform sampler2D baseTexture;

void main() {
    vec3 ambient = AMBIENT * lightColor;

    vec3 lightDir = LIGHT_DIRECTION;

    float specularStrength = 1.0;
    vec3 viewDir = normalize(camera - FragPos);
    vec3 reflectDir = reflect(lightDir, Normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    float diffuse = max(dot(Normal, -lightDir) / 2.0 + 0.5, 0.0);

    vec3 result = (ambient + diffuse + specular) * texture(baseTexture, TexCoords).xyz;

    FragColor = vec4(result, 1.0);
}