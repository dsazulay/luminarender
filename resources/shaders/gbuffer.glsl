#properties
Color u_color = 1,1,1,1
2D u_mainTex = white
#endproperties

#shader vertex
#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out Varyings
{
    vec3 normal;
    vec3 worldPos;
    vec2 uv;
} v_out;


layout (std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
    vec4 u_viewPos;
    vec4 u_camForward;
};

uniform mat4 u_model;
uniform mat3 u_normalMatrix;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1);
    v_out.normal = u_normalMatrix * a_normal;
    v_out.worldPos = vec3(u_model * vec4(a_position, 1));
    v_out.uv = a_uv;
}

#shader fragment
#version 410 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedoSpec;

in Varyings
{
    vec3 normal;
    vec3 worldPos;
    vec2 uv;
} v_in;

uniform vec4 u_color;
uniform sampler2D u_mainTex;

void main()
{
    g_position = v_in.worldPos;
    g_normal = normalize(v_in.normal);
    vec4 mainTex = texture(u_mainTex, v_in.uv) * u_color;
    g_albedoSpec.rgb = mainTex.rgb;
    g_albedoSpec.w = 1;
}
