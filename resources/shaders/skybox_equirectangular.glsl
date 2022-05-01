#shader vertex
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

#shader fragment
#version 410 core
in vec3 v_texcoords;

out vec4 fragColor;

uniform sampler2D u_equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = sampleSphericalMap(normalize(v_texcoords));
    vec3 color = texture(u_equirectangularMap, uv).rgb;

    fragColor = vec4(color, 1.0);
}