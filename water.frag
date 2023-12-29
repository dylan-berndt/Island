#version 330 core
out vec4 FragColor;

in vec3 FragPos;

#define WATER_COLOR vec3(0.1, 0.6, 0.8)

#define AMBIENT_LIGHT 0.1
#define LIGHT_DIRECTION normalize(vec3(1.0, -1.0, 1.0))

uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform float time;

#define SAMPLES 3

const float[SAMPLES] weights = float[SAMPLES](1.0, 0.5, 0.125);
const float[SAMPLES] scales  = float[SAMPLES](0.5, 1.0, 2.0);
const float[SAMPLES] freqs   = float[SAMPLES](0.2, 1.0, 2.2);

float derivative(float v) {
    float sum = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        sum += weights[i] * scales[i] * cos(v * scales[i] + time * freqs[i]);
    }

    return sum;
}

void main() {
    vec3 ambient = AMBIENT_LIGHT * lightColor;

    vec3 lightDir = LIGHT_DIRECTION;

    vec3 t = vec3(1.0, 0.0, derivative(FragPos.x));
    vec3 b = vec3(0.0, 1.0, derivative(FragPos.z));
    vec3 normal = normalize(cross(t, b));

    float diffuse = max(dot(normal, lightDir) / 2.0 + 0.5, 0.0);

    vec3 result = (ambient + diffuse) * WATER_COLOR;
    FragColor = vec4(result, 1.0);
}