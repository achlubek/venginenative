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
    bool disableFaceCull;

    vector<MaterialNode*> nodes;

    void addNode(MaterialNode *node);
};
