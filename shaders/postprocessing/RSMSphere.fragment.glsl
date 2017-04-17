#version 430 core
#define RECREATE_UV
#include PostProcessEffectBase.glsl
#include Shade.glsl
#include ShadeFramework.glsl

in vec3 rsmcolor;
in vec3 rsmposition;
in vec3 fragwpospos;
in vec3 rsmnormal;

vec4 shade(){
    if(distance(CameraPosition, fragwpospos) > currentData.cameraDistance) discard;
    //return vec4(0.01);
    return vec4(shade_ray_data(currentData, normalize(currentData.worldPos - rsmposition), rsmcolor )
    //(1.0 - smoothstep(0.0, 7.0, distance(currentData.worldPos, rsmposition))) *
    //max(0.0, dot(normalize(rsmnormal), normalize(currentData.worldPos - rsmposition))) *
        //max(0.0, dot(normalize(rsmnormal), normalize(currentData.normal))) *
//    max(0.0, dot(normalize(currentData.normal), normalize(currentData.worldPos - rsmposition)))
    , 1.0);
}
