#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint inInstanceId;
layout(location = 2) out vec3 outWorldPos;

layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
    vec2 Resolution;
} hiFreq;
vec3 CameraPosition = hiFreq.inCameraPos.xyz;

struct GeneratedStarInfo {
    vec4 position_radius;
    vec4 color_age; //0->maybe 10? maybe 100?
    vec4 orbitPlane_rotationSpeed;
    vec4 spotsIntensity_zero_zero_zero; //0->1
};

layout(set = 0, binding = 1) buffer StarsStorageBuffer {
    ivec4 count;
    GeneratedStarInfo stars[];
} starsBuffer;

void main() {
    inInstanceId = gl_InstanceIndex;
    vec4 posradius = starsBuffer.stars[gl_InstanceIndex].position_radius;
    posradius.xyz -= CameraPosition;
    float dist = min(300000.0, length(posradius.xyz));
    outWorldPos = normalize(posradius.xyz) * dist + inPosition.xyz * posradius.a * 1.0;
    // + inPosition.xyz * starsBuffer.stars[gl_InstanceIndex].position_radius.a * 7.0;
    vec4 opo = (hiFreq.VPMatrix) * vec4(outWorldPos, 1.0);
    opo.y *= -1.0;
    vec2 newuv = (opo.xy / opo.w) * 0.5 + 0.5;
    outTexCoord = newuv;
    gl_Position = opo; //    gl_Position = vec4(inPosition.xyz, 1.0);
}
