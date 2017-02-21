#include "stdafx.h"
#include "CascadeShadowMap.h"
#include "ShaderProgram.h"
#include "Game.h"

using namespace glm;

CascadeShadowMap::CascadeShadowMap(int width, int height, vector<float> iradiuses)
{
    cascadeCount = iradiuses.size();
    //   texture = new Texture2dArray(width, height, cascadeCount, GL_R32F, GL_RED, GL_FLOAT);
    textureDepth = new Texture2dArray(width, height, cascadeCount, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT);
    framebuffer = new Array2dFramebuffer();
    // framebuffer->attachTexture(texture, GL_COLOR_ATTACHMENT0);
    framebuffer->attachTexture(textureDepth, GL_DEPTH_ATTACHMENT);
    for (int i = 0; i < cascadeCount; i++) {
        float radius = iradiuses[i];
        pmatrices.push_back(glm::ortho(-1, 1, -1, 1, -1, 1));
        farplanes.push_back(radius * 2.0f);
    }
    camera = new Camera();
    radiuses = iradiuses;
}

void CascadeShadowMap::map(glm::vec3 ddirection, glm::vec3 iposition)
{
    position = iposition;
    direction = glm::normalize(ddirection);
    framebuffer->use(false);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    //glDepthFunc(GL_DEPTH_CLAMP);
    //glDisable(GL_CULL_FACE);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    for (int i = 0; i < cascadeCount; i++) {
        mat4 pmat = pmatrices[i];
        framebuffer->use(false);
        framebuffer->switchLayer(i, true);
        camera->projectionMatrix = pmat;
        camera->transformation->setPosition(position);
        camera->transformation->setOrientation(glm::inverse(glm::lookAt(vec3(0), direction, (direction == vec3(0, -1, 0) ? vec3(0, 0, 1) : vec3(0, 1, 0)))));
        camera->transformation->setSize(glm::vec3(radiuses[i]));
        //camera->cone->update(pmat);
        Game::instance->world->setUniforms(Game::instance->shaders->depthOnlyShader, camera);
        Game::instance->shaders->depthOnlyShader->setUniform("CSMRadius", radiuses[i]);
        Game::instance->shaders->depthOnlyShader->setUniform("CSMFarPlane", farplanes[i]);
        Game::instance->world->draw(Game::instance->shaders->depthOnlyShader, camera);
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void CascadeShadowMap::setUniformsAndBindSampler(ShaderProgram * shader, int sampler)
{
    vector<mat4> vpmats = {};
    for (int i = 0; i < cascadeCount; i++) {
        mat4 pmat = pmatrices[i];
        camera->projectionMatrix = pmat;
        camera->transformation->setPosition(position);
        camera->transformation->setOrientation(glm::inverse(glm::lookAt(vec3(0), direction, (direction == vec3(0, -1, 0) ? vec3(0, 0, 1) : vec3(0, 1, 0)))));
        camera->transformation->setSize(glm::vec3(radiuses[i]));
        mat4 inv = camera->transformation->getInverseWorldTransform();
        vpmats.push_back(pmat * inv);
    }
    //shader->setUniformVector("CSMPMatrices", pmatrices);
    shader->setUniform("CSMCenter", position);
    shader->setUniformVector("CSMVPMatrices", vpmats);
    shader->setUniform("CSMLayers", cascadeCount);
    // shader->setUniformVector("CSMFarplanes", farplanes);
    shader->setUniformVector("CSMRadiuses", radiuses);
    textureDepth->use(sampler);
}

CascadeShadowMap::~CascadeShadowMap()
{
}