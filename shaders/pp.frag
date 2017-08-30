#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texDiffuse;
layout(binding = 3) uniform sampler2D texNormal;
layout(binding = 4) uniform sampler2D texDistance;

layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
} hiFreq;

#define CameraPosition (hiFreq.inCameraPos.xyz)
#define FrustumConeLeftBottom (hiFreq.inFrustumConeLeftBottom.xyz)
#define FrustumConeBottomLeftToBottomRight (hiFreq.inFrustumConeBottomLeftToBottomRight.xyz)
#define FrustumConeBottomLeftToTopLeft (hiFreq.inFrustumConeBottomLeftToTopLeft.xyz)

layout(set = 0, binding = 1) uniform UniformBufferObject2 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
} lowFreq;

#include proceduralValueNoise.glsl
#include planet.glsl
#include camera.glsl
#include atmScattering.glsl

float fresnelf(vec3 direction, vec3 normal) {
    vec3 nDirection = normalize( direction );
    vec3 nNormal = normalize( normal );
    vec3 halfDirection = normalize( nNormal + nDirection );

    float cosine = dot( halfDirection, nDirection );
    float product = max( cosine, 0.0 );
    float factor = pow( product, 5.0 );

    return factor;
}

void main() {
    vec4 bb = texture(texDiffuse, inTexCoord).rgba;
    vec3 normal = normalize(texture(texNormal, inTexCoord).rgb);//normalize(bb.rgb);
    vec3 dir = reconstructCameraSpaceDistance(inTexCoord, 1.0);
    float fresnel = 0.04 + 0.96 * fresnelf(dir, normal);
    //vec3 atm = getAtmosphereForDirectionReal(VECTOR_UP * 200.0, dir, normalize(vec3(1.0)));
    //vec3 reflected = 0.04 * bb.rgb * (1.0 - fresnel) + bb.rgb * fresnel * getAtmosphereForDirectionReal(VECTOR_UP * 200.0, reflect(dir, normal), normalize(vec3(1.0)));
    //vec3 res = mix(atm, reflected, step(0.001, texture(texDistance, inTexCoord).r));
    vec3 res = mix(vec3(0.5), bb.rgb, step(0.001, texture(texDistance, inTexCoord).r));
    outColor = vec4(pow(res, vec3(1.0 / 2.2)), 1.0);
}
