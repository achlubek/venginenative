#pragma once
#include "Texture2d.h";
#include "MaterialNode.h";
class Material
{
public:
    Material();
    ~Material();

    glm::vec3 diffuseColor;
    float roughness;
    float metalness;
    Texture2d* diffuseColorTex;
    Texture2d* normalTex;
    Texture2d* bumpTex;
    Texture2d* roughnessTex;
    Texture2d* metalnessTex;
    glm::vec2 diffuseColorTexScale;
    glm::vec2 normalTexScale;
    glm::vec2 bumpTexScale;
    glm::vec2 roughnessTexScale;
    glm::vec2 metalnessTexScale;
};
