@ctype mat4 glm::mat4

@vs vs

layout(binding = 0) uniform instance_params {
    mat4 mvp;
};

in vec2 aPos;
in vec2 aUV;
in vec2 aOffset;
in vec2 aUVOffset;
in vec2 aWorldScale;
in vec2 aUVScale;
in float aRotation;
in vec2 aPivot;
in vec4 aTint;

out vec2 vUV;
out vec4 vTint;

void main() {
    float c = cos(aRotation);
    float s = sin(aRotation);
    mat2 rot = mat2(c, -s, s, c);

    vec2 p = aPos - aPivot;
    vec2 scaled = p * aWorldScale;
    vec2 rotated = rot * scaled;

    vec2 world_xy = rotated + aOffset;
    gl_Position = mvp * vec4(world_xy, 0.0, 1.0);
    
    vUV = aUVOffset + (aUV * aUVScale);

    vTint = aTint;
}
@end

@fs fs
out vec4 FragColor;

in vec2 vUV;
in vec4 vTint;

layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler smp;
#define ourTexture sampler2D(tex, smp)

void main() {
    FragColor = vTint * texture(ourTexture, vUV);
}
@end

@program instance vs fs
