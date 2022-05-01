#shader vertex
#version 410 core
layout (location = 0) in vec3 a_pos;

out vec3 v_texcoords;

layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
};

void main()
{
    v_texcoords = a_pos;
    mat4 view = u_view;
    view[3] = vec4(0.0);
    vec4 pos = u_projection * view * vec4(a_pos, 1.0);
    gl_Position = pos.xyww;
}

#shader fragment
#version 410 core
in vec3 v_texcoords;

out vec4 fragColor;

uniform samplerCube u_mainTex;

void main()
{
    vec3 envColor = texture(u_mainTex, v_texcoords).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    //    envColor = pow(envColor, vec3(1.0/2.2));

    fragColor = vec4(envColor, 1.0);
}
