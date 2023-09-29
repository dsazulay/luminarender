float hash21(vec2 p)
{
    p = fract(p * vec2(123.234, 234.345));
    p += dot(p, p + 213.42);
    return fract(p.x * p.y) - 0.5;
}

float shadowCalculation(vec4 fragPosLightSpace, vec3 lightPos, vec3 worldPos, vec3 normalWS)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normalWS);
    vec3 lightDir = normalize(lightPos - worldPos);
    float NdotL = dot(normal, lightDir);
    float bias = max(0.05 * (1.0 - NdotL), 0.005);
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    int halfSample = 2;
    for(int x = -halfSample; x <= halfSample; ++x)
    {
        for(int y = -halfSample; y <= halfSample; ++y)
        {
            vec2 seed = vec2(x, y) * currentDepth;
            vec2 offset = (vec2(x,y) + vec2(hash21(seed)));
            float pcfDepth = texture(u_shadowMap, projCoords.xy + offset * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= (halfSample * 2.0 + 1.0) * (halfSample * 2.0 + 1.0);

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

// TODO: this is only to avoid compilation errors with the old system
// TODO: remove this
float shadowCalculation(vec4 fragPosLightSpace)
{
    return 1.0;
}

