#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

//#########//

void main() {
    vec4 ui = texture(texUi, UV).rgba;
    vec3 render = 0.0*texture(texAmbient, UV).rgb;
    render += textureLod(texDeferredResolved, UV, 0.0).rgb;
    outColor = vec4(pow( mix(render, ui.rgb, ui.a), vec3(1.0 / 2.2)), 1.0);
    if(UV.x < 0.1 && UV.y < 0.1) outColor = vec4(texture(texAboveDistance, UV * 10.0).r * 0.3);
}
