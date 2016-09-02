#include "stdafx.h"
#include "CascadeShadowMap.h"
#include "ShaderProgram.h"
#include "Game.h"

using namespace glm;

CascadeShadowMap::CascadeShadowMap(int width, int height, vector<float> iradiuses)
{
    cascadeCount = iradiuses.size();
    texture = new Texture2dArray(width, height, cascadeCount, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT);
    framebuffer = new Array2dFramebuffer();
    framebuffer->attachTexture(texture, GL_DEPTH_ATTACHMENT);
    for (int i = 0; i < cascadeCount; i++) {
        float radius = iradiuses[i];
        pmatrices.push_back(glm::ortho(-radius, radius, -radius, radius, -1000.0f, 1000.0f));
        farplanes.push_back(radius * 2.0f);
    } 
    camera = new Camera();
    radiuses = iradiuses;

}

void CascadeShadowMap::map(glm::vec3 direction, glm::vec3 iposition)
{
    position = iposition;
    camera->transformation->setPosition(position);
    camera->transformation->setOrientation(glm::lookAt(position, position + direction, (direction == vec3(0, -1, 0) ? vec3(0, 0, 1) : vec3(0, 1, 0))));
    for (int i = 0; i < cascadeCount; i++) {
        mat4 pmat = pmatrices[i];
        framebuffer->switchLayer(i, true);
        camera->projectionMatrix = pmat;
        camera->cone->update(pmat);
        ShaderProgram *shader = Game::instance->shaders->depthOnlyShader;
        shader->use();
        shader->setUniform("CutOffDistance", 10000.0f);
        Game::instance->world->setUniforms(shader, camera);

        shader = Game::instance->shaders->depthOnlyGeometryShader;
        shader->use();
        shader->setUniform("CutOffDistance", 10000.0f);
        Game::instance->world->setUniforms(shader, camera);
        framebuffer->use(false);
        Game::instance->world->draw(shader, camera);
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
    glm::mat4 cameraViewMatrix = camera->transformation->getWorldTransform();
    shader->setUniform("CSMVMatrix", cameraViewMatrix);
    texture->use(sampler);
}

CascadeShadowMap::~CascadeShadowMap()
{
}
