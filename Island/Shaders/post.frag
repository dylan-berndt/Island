#version 330 core

#define PI 3.14159

in vec2 TexCoord;

out vec4 FragColor;

uniform Texture color;
uniform Texture depth;

#define LOW_FOG vec3(0.6, 0.9, 1.0)
#define HIGH_FOG vec3(1.6, 1.6, 1.6)

float fogDensity = 1.5;
float fogDistance = 400.0;

float bloomEffect = 0.5;
uniform int bloomKernelSize = 4;
float bloomKernelSizeMultiplier = 4.0;

ivec2 colorSize = textureSize(color.id, 0);
ivec2 depthSize = textureSize(depth.id, 0);

vec2 colorTexelSize = 1.0 / vec2(colorSize);
vec2 depthTexelSize = 1.0 / vec2(depthSize);

uniform float saturation = 0.0;
uniform float value = 0.0;
uniform float hue = 0.0;

uniform int pixelSize = 1;

uniform float outlineThickness = 0.0;

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

vec3 fog(vec3 original, vec3 pos) {
    float depthSpread = max(1.0 - (length(pos - camera) / fogDistance), 0.0);

    vec3 current_fog = mix(LOW_FOG, HIGH_FOG, max(normalize(pos - camera).y, 0.0));
    current_fog *= lightColor;

    float heightMult = max(min(1.0 - pos.y / fogDistance, 1.0), 0.0);

    if (length(pos - camera) > fogDistance && 1.0 - pos.y / fogDistance > 0.98) {
        return current_fog;
    }

    float fogFactor = exp(-fogDensity * fogDensity * depthSpread * depthSpread);
    vec3 fogged = mix(original, current_fog, fogFactor * heightMult);

    return fogged;
}

vec3 RGBtoHSV(vec3 col) {
    float cmax = max(max(col.r, col.g), col.b);
    float cmin = min(min(col.r, col.g), col.b);
    float delta = cmax - cmin;

    float h = 0.0;
    if (delta > 0.0) {
        if (cmax == col.r) {
            h = mod((col.g - col.b) / delta, 6.0);
        } else if (cmax == col.g) {
            h = ((col.b - col.r) / delta) + 2.0;
        } else {
            h = ((col.r - col.g) / delta) + 4.0;
        }
    }
    h = h * 60.0;

    float s = (cmax > 0.0) ? (delta / cmax) : 0.0;
    float v = cmax;

    return vec3(h, s, v);
}

vec3 HSVtoRGB(vec3 col) {
    float c = col.z * col.y;
    float x = c * (1.0 - abs(mod(col.x / 60.0, 2.0) - 1.0));
    float m = col.z - c;

    vec3 rgb;
    if (col.x < 60.0) {
        rgb = vec3(c, x, 0.0);
    } else if (col.x < 120.0) {
        rgb = vec3(x, c, 0.0);
    } else if (col.x < 180.0) {
        rgb = vec3(0.0, c, x);
    } else if (col.x < 240.0) {
        rgb = vec3(0.0, x, c);
    } else if (col.x < 300.0) {
        rgb = vec3(x, 0.0, c);
    } else {
        rgb = vec3(c, 0.0, x);
    }
    return rgb + m;
}

vec3 outline(vec3 col, vec2 pos, Texture depthTexture) {
    vec2 diff = vec2(0.0);

    float distance = linearDepth(texture(depthTexture.id, pos).x);

    int y = 0;
    for (int i = 0; i < 4; i++) {
        int x = i < 2 ? (i - 1) : (3 - i);
        int y = ((i + 1) % 4) < 2 ? (((i + 1) % 4) - 1) : (3 - ((i + 1) % 4));
        if (x == 0 && y == 0) {
            continue;
        }
        vec2 samplePos = pos + depthTexelSize * vec2(x, y) * outlineThickness / 2.0;
        diff += vec2(-x, -y) * linearDepth(texture(depthTexture.id, samplePos).x);
    }

    float d = length(diff);
    d = smoothstep(0.0, 0.1, d - distance);
    return mix(col, vec3(0.0), d);
}

vec3 adjust(vec3 col) {
    vec3 HSV = RGBtoHSV(col);

    HSV += vec3(hue * 180.0, saturation, value);

    vec3 RGB = HSVtoRGB(HSV);

    return RGB;
}

void main() {
    vec2 pixelated = floor(TexCoord * colorSize / float(pixelSize)) * (float(pixelSize) / colorSize);

    vec3 original = texture(color.id, pixelated).xyz;
    vec3 pos = worldPosition(pixelated, texture(depth.id, pixelated).x);

    vec3 bloomed = bloom(original, pixelated, color);

    vec3 fogged = fog(bloomed, pos);

    vec3 col = fogged;

    col = adjust(col);
    col = outline(col, pixelated, depth);

    FragColor = vec4(col, 1.0);
}

