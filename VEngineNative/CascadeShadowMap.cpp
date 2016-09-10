#include "stdafx.h"
#include "CascadeShadowMap.h"
#include "ShaderProgram.h"
#include "Game.h"

using namespace glm;

CascadeShadowMap::CascadeShadowMap(int width, int height, vector<float> iradiuses)
{
    cascadeCount = iradiuses.size();
    texture = new Texture2dArray(width, height, cascadeCount, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT);
    framebuffer = new Array2dFramebuffer();
    framebuffer->attachTexture(texture, GL_DEPTH_ATTACHMENT);
    for (int i = 0; i < cascadeCount; i++) {
        float radius = iradiuses[i];
        pmatrices.push_back(glm::ortho(-radius, radius, -radius, radius, -radius, radius));
        farplanes.push_back(radius * 2.0f);
    } 
    camera = new Camera();
    radiuses = iradiuses;

}

void CascadeShadowMap::map(glm::vec3 direction, glm::vec3 iposition)
{
    position = iposition;
    direction = glm::normalize(direction);
    framebuffer->use(false);
    for (int i = 0; i < cascadeCount; i++) {
        mat4 pmat = pmatrices[i];
        framebuffer->use(false);
        framebuffer->switchLayer(i, true);
        camera->projectionMatrix = pmat;
        camera->transformation->setPosition(vec3(0));
        camera->transformation->setOrientation( (glm::lookAt(vec3(0), direction, (direction == vec3(0, -1, 0) ? vec3(0, 0, 1) : vec3(0, 1, 0)))));
        //camera->cone->update(pmat);
        Game::instance->world->setUniforms(Game::instance->shaders->depthOnlyGeometryShader, camera);
        Game::instance->shaders->depthOnlyGeometryShader->setUniform("CSMRadius", radiuses[i]);
        Game::instance->world->setUniforms(Game::instance->shaders->depthOnlyShader, camera);
        Game::instance->shaders->depthOnlyShader->setUniform("CSMRadius", radiuses[i]);
        Game::instance->world->draw(Game::instance->shaders->depthOnlyShader, camera);
    }
}

void CascadeShadowMap::setUniformsAndBindSampler(ShaderProgram * shader, int sampler)
{
    shader->setUniform("CSMLayers", cascadeCount);
    vector<mat4> vpmats = {};
    mat4 inv = camera->transformation->getInverseWorldTransform();
    for (int i = 0; i < cascadeCount; i++) { 
        vpmats.push_back(pmatrices[i] * inv);
    }
    shader->setUniformVector("CSMPMatrices", pmatrices);
    shader->setUniformVector("CSMVPMatrices", vpmats);
    shader->setUniformVector("CSMFarplanes", farplanes);
    shader->setUniformVector("CSMRadiuses", radiuses);
    shader->setUniformVector("CSMCenter", radiuses);
    texture->use(sampler);
}

CascadeShadowMap::~CascadeShadowMap()
{
}
