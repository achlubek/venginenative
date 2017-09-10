#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

#include uiBuffer.glsl

layout(set = 0, binding = 1) uniform sampler2D texBitmap;

void main() {
    if(uiType == UI_TYPE_BITMAP) {
        outColor = textureLod(texBitmap, uiBitmapOffset + UV * uiBitmapScale, 0.0);
    }
    else if(uiType == UI_TYPE_BOX) {
        outColor = uiBoxColor;
    }
    else if(uiType == UI_TYPE_TEXT) {
        outColor = vec4(uiBoxColor.rgb, uiBoxColor.a * textureLod(texBitmap, uiBitmapOffset + UV * uiBitmapScale, 0.0).r);
    } else {
        outColor = vec4(1.0);
    }
}
