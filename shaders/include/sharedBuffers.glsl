#pragma once

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
