#pragma once
#include "ShaderProgram.h"
class GenericShaders
{
public:
    GenericShaders();
    ~GenericShaders();
    ShaderProgram *materialShader;
    ShaderProgram *depthOnlyShader;
    ShaderProgram *idWriteShader;
	ShaderProgram *sunRSMWriteShader;
	ShaderProgram *aboveViewShader;
};
