// For 3d models

@ctype mat4 glm::mat4
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4

@vs vs
in vec3 a_pos;
in vec3 a_color;

out vec3 v_color;

layout(binding = 0) uniform vs_params {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    v_color = a_color;
}
@end

@fs fs
in vec3 v_color;

out vec4 frag_color;

layout(binding = 1) uniform fs_params {
    vec4 tint;
};

float gamma_mult = 3.0;

void main() {
    // for now bump gamma, later do lighting
    frag_color = tint * vec4(v_color * gamma_mult, 1.0);
}
@end

@program model vs fs


// next
@vs vs_tex
in vec3 a_pos;
in vec2 a_tex_coords;

out vec2 tex_coords;

layout(binding = 2) uniform vs_tex_params {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    tex_coords = a_tex_coords;
}
@end

@fs fs_tex
in vec2 tex_coords;

out vec4 frag_color;

layout(binding = 3) uniform texture2D _diffuse_texture;
layout(binding = 3) uniform sampler diffuse_texture_smp;
#define diffuse_texture sampler2D(_diffuse_texture, diffuse_texture_smp)

layout(binding = 4) uniform fs_tex_params {
    vec4 tint;
};

void main() {
    frag_color = tint * texture(diffuse_texture, tex_coords);
}
@end

@program model_tex vs_tex fs_tex
