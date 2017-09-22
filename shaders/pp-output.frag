#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

//#########//

void main() {
    vec4 ui = texture(texUi, UV).rgba;
    vec3 render = 0.05 * texture(texAmbient, UV).rgb;
    render += textureLod(texDeferredResolved, UV, 0.0).rgb;
    outColor = vec4(pow(mix(render, ui.rgb, ui.a), vec3(1.0 / 2.2)), 1.0);
}
