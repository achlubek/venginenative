#ifndef SHADEFRAMEWORK_H
#define SHADEFRAMEWORK_H
#include Constants.glsl

layout(binding = 8) uniform sampler2D fresnelTex;
vec3 fresnel_effect(vec3 base, float roughness, float dt){
    return base + (1.0 - base) * textureLod(fresnelTex, vec2(clamp(roughness, 0.01, 0.98), clamp(1.0 - dt, 0.01, 0.98)), 0.0).r;
}

float really_phong(float dt, float roughness){
    float gloss = pow(1.0 - roughness, 10.0) ;
    return pow(max(0.0, dt - 0.9), gloss * gloss * 4256.0 + 1.0);
}

vec3 shade_ray(vec3 albedo, vec3 normal, vec3 viewdir, float roughness, float metalness, vec3 lightdir, vec3 lightcolor){

    float dotNL = max(0.0, dot(normal,lightdir));
    vec3 refl = normalize(reflect(viewdir, normal));
    float dotRefL = max(0.0, dot(normalize(mix(refl, normal, roughness * roughness)), lightdir));
    float dotNV = max(0.0, dot(normal,-viewdir));
    float invdotNV = max(0.0, dot(-lightdir,viewdir));
    vec3 spec_color_nonmetal = (1.0 - roughness) * lightcolor * fresnel_effect(vec3(0.04), roughness * 0.99 + 0.01, dotNV) * LightingFuncGGX_REF(normalize(normal), -viewdir, lightdir, roughness * 0.96 + 0.04, vec3(1.0));
    vec3 spec_color_metal = lightcolor * fresnel_effect(albedo, roughness * 0.99 + 0.01, dotNV) * LightingFuncGGX_REF(normalize(normal), -viewdir, lightdir, roughness * 0.98 + 0.02, albedo);
    vec3 diffuse_color_nonmetal = albedo * lightcolor * LightingFuncGGX_REF(normal, -viewdir, lightdir, 1.0, albedo) ;
    //return vec3(0.0);
    return mix(spec_color_nonmetal + diffuse_color_nonmetal, spec_color_metal, metalness);
}

vec3 shade_ray_env(vec3 albedo, vec3 normal, vec3 viewdir, float roughness, float metalness, vec3 lightdir, vec3 lightcolor, vec3 lightdiffusecolor){

    float dotNL = max(0.0, dot(normal,lightdir));
    vec3 refl = normalize(reflect(viewdir, normal));
    float dotRefL = max(0.0, dot(normalize(mix(refl, normal, roughness * roughness)), lightdir));
    float dotNV = max(0.0, dot(normal,-viewdir));
    float invdotNV = max(0.0, dot(-lightdir,viewdir));

    vec3 spec_color_nonmetal =  (1.0 - roughness) * lightcolor * fresnel_effect(vec3(0.04), roughness * 0.99 + 0.01, dotNV);
    vec3 spec_color_metal = lightcolor * fresnel_effect(albedo, roughness * 0.99 + 0.01, dotNV);
    vec3 diffuse_color_nonmetal = albedo * lightdiffusecolor;
//    return vec3(0.0);
//return diffuse_color_nonmetal;
    return mix(spec_color_nonmetal + diffuse_color_nonmetal, spec_color_metal, metalness);
}

vec3 shade_ray_data(PostProcessingData data, vec3 lightdir, vec3 lightcolor){

    return shade_ray(data.diffuseColor, data.normal, normalize(data.cameraPos), data.roughness, data.metalness, lightdir, lightcolor);
}

vec3 shade_ray_env_data(PostProcessingData data, vec3 lightdir, vec3 lightcolor, vec3 lightdiffusecolor){

    return shade_ray_env(data.diffuseColor, data.normal, normalize(data.cameraPos), data.roughness, data.metalness, lightdir, lightcolor, lightdiffusecolor);
}
#endif
