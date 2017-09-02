#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};
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


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexCoord;


void main() {
    gl_Position = vec4(inPosition.xyz, 1.0);
    outNormal = inNormal;
    outTexCoord = inTexCoord;
}
