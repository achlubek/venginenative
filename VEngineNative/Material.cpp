#include "stdafx.h"
#include "Material.h"

using namespace glm;

Material::Material()
{
    diffuseColorTex = nullptr;
    normalTex = nullptr;
    bumpTex = nullptr;
    roughnessTex = nullptr;
    metalnessTex = nullptr;

    diffuseColor = vec3(1);
    roughness = 1.0;
    metalness = 0.0;

    diffuseColorTexScale = glm::vec2(1.0f);
    normalTexScale = glm::vec2(1.0f);
    bumpTexScale = glm::vec2(1.0f);
    roughnessTexScale = glm::vec2(1.0f);
    metalnessTexScale = glm::vec2(1.0f);
}

Material::~Material()
{
    // texture2d needs refcounters because reuse is important
}