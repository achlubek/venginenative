#pragma once

layout(location = 0) in vec2 UV;

layout(set = 0, binding = 2) uniform sampler2D texDiffuse;
layout(set = 0, binding = 3) uniform sampler2D texNormal;
layout(set = 0, binding = 4) uniform sampler2D texDistance;

layout(set = 0, binding = 5) uniform sampler2D texAmbient;
layout(set = 0, binding = 6) uniform sampler2D texUi;
layout(set = 0, binding = 7) uniform sampler2D texDeferredResolved;

#include sharedBuffers.glsl
