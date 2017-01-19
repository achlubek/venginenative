#include "stdafx.h"
#include "MaterialAssembly.h"
#include "ShaderProgram.h"
#include "Game.h"


MaterialAssembly::MaterialAssembly()
{
    uniformsFloat = vector<float>();
    uniformsInt = vector<int>();
    uniformsVec2 = vector<glm::vec2>();
    uniformsVec3 = vector<glm::vec3>();
    uniformsVec4 = vector<glm::vec4>();
    programAssembly = vector<int>();
    textures = vector<Texture2d*>();
    texturesScales = vector<glm::vec2>();
}


MaterialAssembly::~MaterialAssembly()
{
}

void MaterialAssembly::setUniforms()
{
    auto s = ShaderProgram::current;
    s->setUniformVector("AsmUniformsFloat", uniformsFloat);
    s->setUniformVector("AsmUniformsInt", uniformsInt);
    s->setUniformVector("AsmUniformsVec2", uniformsVec2);
    s->setUniformVector("AsmUniformsVec3", uniformsVec3);
    s->setUniformVector("AsmUniformsVec4", uniformsVec4);
    s->setUniformVector("AsmProgram", programAssembly);
    s->setUniformVector("TexturesScales", texturesScales);
    s->setUniform("AsmProgramLength", (int)programAssembly.size());
    s->setUniform("TexturesCount", (int)textures.size());
    for (int i = 0; i < textures.size(); i++) {
        textures[i]->use(i);
    }
}
