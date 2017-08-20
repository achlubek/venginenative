#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    float Time;
    float Zero;
    vec2 Mouse;
} ubo;
void main() {
    outColor = vec4(texture(texSampler, inTexCoord).rgb * (sin(ubo.Time) * 0.5 + 0.5), 1.0);
}
