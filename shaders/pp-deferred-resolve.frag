#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

layout(set = 1, binding = 0) uniform shadowmapubo {
    mat4 ViewMatrix;
    vec4 Position;
    vec4 Color;
} lightData;
#define isShadowMappingEnabled (lightData.Position.a > 0.5)
layout(set = 1, binding = 1) uniform sampler2D shadowMap;

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

void main() {
    vec3 diffuse = texture(texDiffuse, UV).rgb;
    vec3 normal = normalize(texture(texNormal, UV).rgb);

    vec3 worldPos = FromCameraSpace(reconstructCameraSpaceDistance(UV, texture(texDistance, UV).r));

    vec2 lightuv = project(worldPos);
    float shadowdata = textureLod(shadowMap, lightuv, 0.0).r;

    float expected = distance(lightData.Position.xyz, worldPos);
    vec3 to_light_dir = normalize(lightData.Position.xyz - worldPos);
    float dt = max(0.0, dot(normal, to_light_dir));
    float shadow = 1.0;
    if(isShadowMappingEnabled){
        shadow = step(0.0, lightuv.x)
            * step(0.0, lightuv.y)
            * (1.0 - step(1.0, lightuv.x))
            * (1.0 - step(1.0, lightuv.x))
            * clip(worldPos)
            * (1.0 - smoothstep(0.0, 0.001, expected - shadowdata));
    }

    vec3 attenuated_diffuse = diffuse / (expected*expected+1.0);

    outColor = vec4(dt * shadow * attenuated_diffuse * lightData.Color.xyz, 1.0);
}
