#version 330 core
out vec4 FragColor;

#define AMBIENT 0.6

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;

uniform vec3 baseColor = vec3(1.0, 0.0, 1.0);

uniform Texture baseTexture;
uniform Texture bumpTexture;

void main() {
    vec2 baseTransformCoords = baseTexture.scale.xy * (TexCoords + baseTexture.offset.xy);
    vec2 bumpTransformCoords = bumpTexture.scale.xy * (TexCoords + bumpTexture.offset.xy);

    if (texture(baseTexture.id, baseTransformCoords).a < 0.2) {
        gl_FragDepth = 1.0;
    }
    else {
        vec4 v_clip_coord = projection * view * vec4(FragPos, 1.0);
        float f_ndc_depth = v_clip_coord.z / v_clip_coord.w;
        gl_FragDepth = (1.0 - 0.0) * 0.5 * f_ndc_depth + (1.0 + 0.0) * 0.5;
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

    float shade = shadow(FragPos, 3) * 0.6;

    vec4 res = vec4(ambient + (1.0 - shade) * (diffuse + specular), 1.0) * texture(baseTexture.id, baseTransformCoords);

    FragColor = res;
}