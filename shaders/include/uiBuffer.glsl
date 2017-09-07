#pragma once
layout(set = 0, binding = 0) uniform ubo {
    vec4 position_scale;
    vec4 box_color;
    vec4 bitmap_uv_offset_scale;
    ivec4 type;
} dataBuffer;
vec2 uiPosition = dataBuffer.position_scale.xy;
vec2 uiScale = dataBuffer.position_scale.zw;
vec2 uiBitmapOffset = dataBuffer.bitmap_uv_offset_scale.xy;
vec2 uiBitmapScale = dataBuffer.bitmap_uv_offset_scale.zw;
vec4 uiBoxColor = dataBuffer.box_color.rgba;
int uiType = dataBuffer.type.x;
#define UI_TYPE_BOX 1
#define UI_TYPE_BITMAP 2
#define UI_TYPE_TEXT 3
