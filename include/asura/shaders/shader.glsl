@ctype mat4 glm::mat4

@vs vs_inst

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

@fs fs_inst
out vec4 FragColor;

in vec2 vUV;
in vec4 vTint;

layout(binding = 0) uniform texture2D inst_tex;
layout(binding = 0) uniform sampler inst_smp;
#define inst_texture sampler2D(inst_tex, inst_smp)

void main() {
    FragColor = vTint * texture(inst_texture, vUV);
}
@end

@program instance vs_inst fs_inst

@vs vs_text

layout(binding = 0) uniform text_params {
    mat4 mvp;
};

in vec2 position;
in vec2 texcoord0;
in vec4 color0;

out vec2 uv;
out vec4 color;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);  // if pass already in clip space?
    uv = texcoord0;
    color = color0;
}
@end

@fs fs_text
layout(binding = 1) uniform texture2D text_tex;
layout(binding = 1) uniform sampler text_smp;
#define text_texture sampler2D(text_tex, text_smp)

in vec2 uv;
in vec4 color;
out vec4 frag_color;

void main() {
    float alpha = texture(text_texture, uv).r;  // single-channel atlas mayhaps
    // frag_color = vec4(color.rgb, color.a * alpha);
    frag_color = color;
}
@end

@program text vs_text fs_text