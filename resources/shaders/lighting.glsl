struct LightUniform
{
    vec4 posAndCutoff;
    vec4 dirAndOuterCutoff;
    vec4 colorAndIntensity;
    vec4 attenuationAndType;
};

layout (std140) uniform Lights
{
    vec4 nLights;
    LightUniform light[12];
};

struct Light
{
    vec3 direction;
    vec3 color;
    float attenuation;
};

Light getDirLight(LightUniform light);
Light getPointLight(LightUniform light, vec3 worldPos);
Light getSpotLight(LightUniform light, vec3 worldPos);

int getLightCount()
{
    return int(nLights.x + nLights.y + nLights.z);
}

Light getLight(int i, vec3 worldPos)
{
    if (light[i].attenuationAndType.w == 0)
    {
        return getDirLight(light[i]);
    }
    else if (light[i].attenuationAndType.w == 1)
    {
        return getPointLight(light[i], worldPos);
    }
    else
    {
        return getSpotLight(light[i], worldPos);
    }
}


Light getDirLight(LightUniform light)
{
    Light l;
    l.direction = normalize(-light.dirAndOuterCutoff.xyz);
    l.color = light.colorAndIntensity.xyz * light.colorAndIntensity.w;
    l.attenuation = 1.0;

    return l;
}

Light getPointLight(LightUniform light, vec3 worldPos)
{
    Light l;
    l.direction = normalize(light.posAndCutoff.xyz - worldPos);
    l.color = light.colorAndIntensity.xyz * light.colorAndIntensity.w;

    float dist = length(light.posAndCutoff.xyz - worldPos);
    float attenuation = 1.0 / (dist * dist);
    l.attenuation = attenuation;

    return l;
}

Light getSpotLight(LightUniform light, vec3 worldPos)
{
    Light l;
    l.direction = normalize(light.posAndCutoff.xyz - worldPos);
    l.color = light.colorAndIntensity.xyz * light.colorAndIntensity.w;

    float dist = length(light.posAndCutoff.xyz - worldPos);
    float attenuation = 1.0 / (light.attenuationAndType.x + light.attenuationAndType.y * dist + light.attenuationAndType.z * dist * dist);
    float cutoff = light.posAndCutoff.w;
    float outerCutoff = light.dirAndOuterCutoff.w;
    float theta = dot(l.direction, normalize(-light.dirAndOuterCutoff.xyz));
    float epsilon = cutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
    l.attenuation = attenuation * intensity;

    return l;
}