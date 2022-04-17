#version 410 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec2 v_texcoords;

void main() {
    v_texcoords = a_uv;
    gl_Position = vec4(a_pos, 1);
}
