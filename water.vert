#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

out vec3 FragPos;

#define SAMPLES 3

const float[SAMPLES] weights = float[SAMPLES](1.0, 0.5, 0.125);
const float[SAMPLES] scales  = float[SAMPLES](0.5, 1.0, 2.0);
const float[SAMPLES] freqs   = float[SAMPLES](0.2, 1.0, 2.2);

float value(float v) {
    float sum = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        sum += weights[i] * sin(v * scales[i] + time * freqs[i]);
    }

    return sum;
}

float displace(vec3 position) {
    return value(position.x) + value(position.z);
}

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);

    vec4 shift = vec4(0.0, displace(aPos), 0.0, 0.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0) + shift;
}
