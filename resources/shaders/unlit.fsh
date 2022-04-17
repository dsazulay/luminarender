#version 410 core

#include "lighting.glsl"

in vec2 v_uv;

out vec4 fragColor;

uniform vec4 u_color;
uniform sampler2D u_mainTex;

void main()
{
    fragColor = texture(u_mainTex, v_uv);
}
