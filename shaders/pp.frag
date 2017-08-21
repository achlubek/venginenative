#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;

layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
} hiFreq;
layout(set = 0, binding = 1) uniform UniformBufferObject2 {
    float Time;
    float Zero;
    vec2 Mouse;
} lowFreq;


void main() {
    outColor = vec4(texture(texSampler, inTexCoord).rgb * hiFreq.Mouse.x * (sin(hiFreq.Time) * 0.5 + 0.5), 1.0);
}
