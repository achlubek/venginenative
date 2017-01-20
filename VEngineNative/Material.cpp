#include "stdafx.h"
#include "Material.h"

using namespace glm;

Material::Material()
{
    diffuseColor = vec3(1);
    roughness = 1.0;
    metalness = 0.0;
    nodes = {};
    disableFaceCull = false;
}

Material::~Material()
{
}

void Material::addNode(MaterialNode * node)
{
    nodes.push_back(node);
}