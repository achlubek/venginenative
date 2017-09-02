#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

//#########//

void main() {
    outColor = vec4(pow(texture(texAmbient, UV).rgb, vec3(1.0 / 2.2)), 1.0);
}
