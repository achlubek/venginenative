#include "stdafx.h"
#include "FrustumCone.h"

using namespace glm;

FrustumCone::FrustumCone()
{
}

FrustumCone::~FrustumCone()
{
}

void FrustumCone::update(mat4 rotprojmatrix)
{
    leftBottom = getDir(vec2(-1, -1), rotprojmatrix);
    rightBottom = getDir(vec2(1, -1), rotprojmatrix);
    leftTop = getDir(vec2(-1, 1), rotprojmatrix);
    rightTop = getDir(vec2(1, 1), rotprojmatrix);
}

glm::vec3 FrustumCone::reconstructDirection(glm::vec2 uv)
{
    return glm::normalize((leftBottom + (rightBottom - leftBottom) * uv.x + (leftTop - leftBottom) * uv.y));

}

vec3 FrustumCone::getDir(vec2 uv, mat4 inv)
{
    vec4 clip = inv * vec4(uv.x, uv.y, 0.1, 1.0);
    return normalize(vec3(clip.x, clip.y, clip.z) / clip.w);
}