#version 410 core
layout (location = 0) in vec3 a_pos;

out vec3 v_texcoords;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    v_texcoords = a_pos;
    gl_Position =  u_projection * u_view * vec4(a_pos, 1.0);
}