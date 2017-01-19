#ifndef SHADEFRAMEWORK_H
#define SHADEFRAMEWORK_H
#include Constants.glsl

layout(binding = 14) uniform sampler2D fresnelTex;
vec3 fresnel_effect(vec3 base, float roughness, float dt){
    return base + (1.0 - base) * textureLod(fresnelTex, vec2(clamp(roughness, 0.01, 0.98), clamp(1.0 - dt, 0.01, 0.98)), 0.0).r;
}

vec3 shade_ray(vec3 albedo, vec3 normal, vec3 viewdir, float roughness, float metalness, vec3 lightdir, vec3 lightcolor){

    float dotNL = max(0.0, dot(normal,lightdir));
    float dotNV = max(0.0, dot(normal,-viewdir));
    float invdotNV = max(0.0, dot(-lightdir,viewdir));

    vec3 spec_color_nonmetal = lightcolor * fresnel_effect(vec3(0.04), roughness, dotNV) * (1.0 - roughness);
    vec3 spec_color_metal = lightcolor * fresnel_effect(albedo, roughness, dotNV);
    vec3 diffuse_color_nonmetal = albedo * dotNL;

    return mix(spec_color_nonmetal + diffuse_color_nonmetal, spec_color_metal, metalness);
}

vec3 shade_ray_data(PostProcessingData data, vec3 lightdir, vec3 lightcolor){

    return shade_ray(data.diffuseColor, data.normal, normalize(data.cameraPos), data.roughness, data.metalness, lightdir, lightcolor);
}
#endif
