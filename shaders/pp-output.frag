#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

#include postProcessingInputs.glsl

//#########//

void main() {
    vec4 ui = texture(texUi, UV).rgba;
    vec3 render = pow(texture(texAmbient, UV).rgb, vec3(1.0 / 2.2));
    if(UV.x < 0.1 && UV.y < 0.1) render = textureLod(shadowMap, UV * 10.0, 0.0).rrr;
    outColor = vec4(mix(render, ui.rgb, ui.a), 1.0);
}
