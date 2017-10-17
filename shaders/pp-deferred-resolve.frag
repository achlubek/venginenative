#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

layout(set = 1, binding = 0) uniform shadowmapubo {
    mat4 ViewMatrix;
    vec4 Position;
    vec4 Color;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
//    vec4 distance_StartXEndY_
} lightData;
#define isShadowMappingEnabled (lightData.Position.a > 0.5)
layout(set = 1, binding = 1) uniform sampler2D shadowMap;

#line 20
vec3 reconstructLightDistance(vec2 uv, float dist){
    uv.y = 1.0 - uv.y;
    vec3 dir = lightData.inFrustumConeLeftBottom.xyz + lightData.inFrustumConeBottomLeftToBottomRight.xyz * uv.x + lightData.inFrustumConeBottomLeftToTopLeft.xyz * uv.y;
    return normalize(dir) * dist;
}

#include camera.glsl

//#########//
float fresnelf(vec3 direction, vec3 normal) {
    vec3 nDirection = normalize( direction );
    vec3 nNormal = normalize( normal );
    vec3 halfDirection = normalize( nNormal + nDirection );

    float cosine = dot( halfDirection, nDirection );
    float product = max( cosine, 0.0 );
    float factor = pow( product, 5.0 );

    return factor;
}

vec2 project(vec3 pos){
    vec4 tmp = (lightData.ViewMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
float clip(vec3 pos){
    vec4 tmp = (lightData.ViewMatrix * vec4(pos, 1.0));
    return step(0.0, tmp.z);
}

float rand2s(vec2 co){
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2sTime(vec2 co){
    return fract(sin(dot(co.xy * hiFreq.Time,vec2(12.9898,78.233))) * 43758.5453);
}
float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{
    return clamp(dot(point - origin, normal) / dot(direction, normal), -1.0, 9991999.0);
}

float getShadow(vec3 camera, vec3 worldpos, vec2 centeruv){
    float expected = distance(camera, worldpos);
    float central = textureLod(shadowMap, centeruv , 0.0).r;
    vec3 pixel = vec3(1.0 / 1024.0, 1.0 / 1024.0, 0.0)  * 10.0;
    float pre = 0.0;
    float prew = 0.0;
    // average only lower than 'expected'
    // this means expression value is always < than expected
    // higher - lower = positive value
    // expected - value
    // afterwards the value would be expected - result
    vec2 seed = UV;
    float scale = 0.01;
    for(int i=0;i<70;i++){
        vec2 newuv = clamp(centeruv + scale * (vec2(rand2s(seed), rand2s(seed + 121.0)) * 2.0 - 1.0), 0.0, 1.0);
        float blocker = textureLod(shadowMap, newuv, 0.0).r;
        pre += max(0.0, expected - blocker) / (blocker + 1.0);
        seed += 12.0;
        prew += 1.0;
    }
    pre /= prew;

    float v = 0.0;
    float vw = 0.0;
    seed = UV;
    vec3 normal = normalize(texture(texNormal, UV).rgb);
    scale = pixel.x + pre * pixel.x * 0.004;
    for(int i=0;i<16;i++){
        vec2 newuv = clamp(centeruv + scale * (vec2(rand2s(seed), rand2s(seed + 121.0)) * 2.0 - 1.0), 0.0, 1.0);
        float shadowdata = textureLod(shadowMap, newuv, 0.0).r;
        vec3 dir = reconstructLightDistance(newuv, 1.0);
        float dst = intersectPlane(lightData.Position.xyz, dir, worldpos, normal);
        v += smoothstep(0.01, 0.02, expected - shadowdata);
        seed += 12.0;
        vw += 1.0;
    }
    v /= vw;
    return 1.0 - v;
}

void main() {
    vec3 diffuse = textureLod(texDiffuse, UV, 0.0 + step(1.5, UV.x)).rgb;
    vec3 normal = normalize(texture(texNormal, UV).rgb);

    vec3 worldPos = FromCameraSpace(reconstructCameraSpaceDistance(UV, texture(texDistance, UV).r));

    vec2 lightuv = project(worldPos);
    float shadowdata = textureLod(shadowMap, lightuv, 0.0).r;

    float expected = distance(lightData.Position.xyz, worldPos);
    vec3 to_light_dir = normalize(lightData.Position.xyz - worldPos);
    float dt = max(0.0, dot(normal, to_light_dir));
    float shadow = 0.0;
    if(isShadowMappingEnabled){
        if(lightuv.x > 0.0 && lightuv.x < 1.0 && lightuv.y > 0.0 && lightuv.y < 1.0){
            shadow = clip(worldPos)
                * getShadow(lightData.Position.xyz, worldPos, lightuv);
        }
    }

    vec3 attenuated_diffuse = diffuse;// / (expected*expected+1.0) ;

    outColor = vec4(dt * shadow * attenuated_diffuse * lightData.Color.xyz, 1.0);
}
