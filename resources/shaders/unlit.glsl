#properties
Color u_color = 1,1,1,1
2D u_mainTex = white
#endproperties

#shader vertex
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec2 v_uv;

layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
};

uniform mat4 u_model;

void main()
{
    v_uv = a_uv;
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}

#shader fragment
#version 410 core

#include "lighting.glsl"

in vec2 v_uv;

out vec4 fragColor;

uniform vec4 u_color;
uniform sampler2D u_mainTex;

void main()
{
    fragColor = texture(u_mainTex, v_uv) * u_color;
}
