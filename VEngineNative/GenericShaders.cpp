#include "stdafx.h"
#include "GenericShaders.h"

GenericShaders::GenericShaders()
{
    materialShader = new ShaderProgram("Generic.vertex.glsl", "Material.fragment.glsl");
    depthOnlyShader = new ShaderProgram("Generic.vertex.glsl", "DepthOnly.fragment.glsl");
    idWriteShader = new ShaderProgram("Generic.vertex.glsl", "IdWrite.fragment.glsl");
}

GenericShaders::~GenericShaders()
{
    delete materialShader;
    delete depthOnlyShader;
    delete idWriteShader;
}