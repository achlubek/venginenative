#pragma once

layout(location = 0) in vec2 UV;

layout(binding = 2) uniform sampler2D texDiffuse;
layout(binding = 3) uniform sampler2D texNormal;
layout(binding = 4) uniform sampler2D texDistance;

layout(binding = 5) uniform sampler2D texAmbient;
layout(binding = 6) uniform sampler2D texUi;

#include sharedBuffers.glsl
