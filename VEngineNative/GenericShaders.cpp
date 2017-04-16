#include "stdafx.h"
#include "GenericShaders.h"

GenericShaders::GenericShaders()
{
    materialShader = new ShaderProgram("Generic.vertex.glsl", "Material.fragment.glsl");
    depthOnlyShader = new ShaderProgram("Generic.vertex.glsl", "DepthOnly.fragment.glsl");
    idWriteShader = new ShaderProgram("Generic.vertex.glsl", "IdWrite.fragment.glsl");
	sunRSMWriteShader = new ShaderProgram("Generic.vertex.glsl", "SunRSMWrite.fragment.glsl");
    aboveViewShader = new ShaderProgram("Generic.vertex.glsl", "AboveView.fragment.glsl");
    voxelWriterShader = new ShaderProgram("Generic.vertex.glsl", "VoxelWriter.fragment.glsl");
}

GenericShaders::~GenericShaders()
{
    delete materialShader;
    delete depthOnlyShader;
    delete idWriteShader;
	delete sunRSMWriteShader;
	delete aboveViewShader;
}