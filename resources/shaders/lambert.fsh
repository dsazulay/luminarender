#version 410 core

in vec3 v_normal;
in vec3 v_worldPos;
in vec2 v_uv;

out vec4 fragColor;

uniform vec4 u_color;
uniform sampler2D u_mainTex;

#include "lighting.glsl"

vec4 lambert(Light light, vec3 normal, vec3 color);

void main()
{
    fragColor = vec4(0, 0, 0, 0);
    vec3 normal = normalize(v_normal);
    vec4 mainTex = texture(u_mainTex, v_uv);

    int lightCount = getLightCount();
    for (int i = 0 ; i < lightCount; i++)
    {
        Light l = getLight(i, v_worldPos);
        fragColor += lambert(l, normal, mainTex.xyz * u_color.rgb);
    }
}

vec4 lambert(Light light, vec3 normal, vec3 color)
{
    float NdotL = max(0.0, dot(normal, light.direction));
    vec3 radiance = color * light.color * NdotL * light.attenuation;

    return vec4(radiance, 1.0);
}
