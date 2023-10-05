#shader vertex
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec2 v_uv;

void main()
{
    v_uv = a_uv;
    gl_Position = vec4(a_position * 2.0, 1.0);
}

#shader fragment
#version 410 core

in vec2 v_uv;

out float fragColor;

uniform sampler2D u_ssao;


void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(u_ssao, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(u_ssao, v_uv + offset).r;
        }
    }
    fragColor = result / (4.0 * 4.0);
}
