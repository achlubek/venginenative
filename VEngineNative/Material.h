#pragma once
#include "Texture2d.h";
#include "MaterialNode.h";
#include "MaterialAssembly.h";
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
    MaterialAssembly* Masm;

    void addNode(MaterialNode *node);
};
