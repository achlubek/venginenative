#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in float inDepth;

layout(location = 0) out vec4 outColor;

layout(binding = 6) uniform sampler2D diffuseTexture;
layout(binding = 7) uniform sampler2D normalTexture;

void main() {
//	float dt = max(0.0, dot(normalize(inNormal), normalize(vec3(1.0, -1.0, 0.0)))) *0.99 + 0.01;
vec3 result = pow(texture(diffuseTexture, inTexCoord).rgb , vec3(2.2));
result = pow(result, vec3(1.0/ 2.2));
    outColor = vec4(result, 1.0);
//    gl_FragDepth = 1.0 - inDepth;
}
