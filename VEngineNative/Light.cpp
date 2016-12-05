#include "stdafx.h"
#include "Game.h"
#include "Light.h"
#include "Camera.h"

Light::Light()
{
    initTransformation();
    shadowMappingEnabled = false;
    shadowMapWidth = 1;
    shadowMapHeight = 1;
    cutOffDistance = 10000;
    angle = deg2rad(90);
    type = LIGHT_SPOT;
    lightCamera = new Camera();
    delete lightCamera->transformation;
    mapper = nullptr;
    cubeMapper = nullptr;
}

Light::~Light()
{
    destroyFbo();
}

void Light::resizeShadowMap(int width, int height)
{
    if (shadowMapWidth == width && shadowMapHeight == height) return;

    shadowMapWidth = width;
    shadowMapHeight = height;

    recreateFbo();
}

void Light::switchShadowMapping(bool value)
{
    if (shadowMappingEnabled == value) return;
    if (value) {
        shadowMappingEnabled = true;
        recreateFbo();
    }
    else {
        shadowMappingEnabled = false;
        destroyFbo();
    }
}

void Light::bindShadowMap(int spot, int cube)
{
    if (shadowMappingEnabled && depthMap != nullptr && depthCubeMap != nullptr) {
        if (type == LIGHT_SPOT) depthMap->use(spot);
        else depthCubeMap->use(cube);
    }
}

void Light::refreshShadowMap()
{
    if (shadowMappingEnabled) {
        glClearDepth(0);
        glDepthFunc(GL_GEQUAL);
        if (type == LIGHT_SPOT) {
            if (mapper != nullptr) {
                mapper->use(true);

                lightCamera->transformation = transformation;
                lightCamera->createProjectionPerspective(angle, (float)shadowMapWidth / (float)shadowMapHeight, 0.1f, cutOffDistance);

                ShaderProgram *shader = Game::instance->shaders->depthOnlyShader;
                shader->use();
                shader->setUniform("CutOffDistance", cutOffDistance);
                Game::instance->world->setUniforms(shader, lightCamera);

                shader = Game::instance->shaders->depthOnlyGeometryShader;
                shader->use();
                shader->setUniform("CutOffDistance", cutOffDistance);
                Game::instance->world->setUniforms(shader, lightCamera);
                Game::instance->world->draw(shader, lightCamera);
            }
        }
        else if (type == LIGHT_POINT) {
            for (int i = 0; i < 6; i++) {
                if (cubeMapper != nullptr) {
                    cubeMapper->use();
                    Camera *cam = cubeMapper->switchFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, true);
                    cam->transformation->setPosition(transformation->position);

                    ShaderProgram *shader = Game::instance->shaders->depthOnlyShader;
                    shader->use();
                    shader->setUniform("CutOffDistance", cutOffDistance);
                    Game::instance->world->setUniforms(shader, cam);

                    shader = Game::instance->shaders->depthOnlyGeometryShader;
                    shader->use();
                    shader->setUniform("CutOffDistance", cutOffDistance);
                    Game::instance->world->setUniforms(shader, cam);

                    Game::instance->world->draw(shader, cam);
                }
            }
        }
        glClearDepth(1);
        glDepthFunc(GL_LEQUAL);
    }
}

void Light::recreateFbo()
{
    Game::instance->invoke([&]() {
        destroyFbo();
        depthMap = new Texture2d(shadowMapWidth, shadowMapHeight,
            GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
        mapper = new Framebuffer();
        mapper->attachTexture(depthMap, GL_DEPTH_ATTACHMENT);

        depthCubeMap = new CubeMapTexture(shadowMapWidth, shadowMapHeight,
            GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
        cubeMapper = new CubeMapFramebuffer();
        cubeMapper->attachTexture(depthCubeMap, GL_DEPTH_ATTACHMENT);
    });
    /*
         mapper->use(true);
         cubeMapper->use();
     });*/
}

void Light::destroyFbo()
{
    if (mapper != nullptr) {
        delete mapper;
        delete depthMap;
        delete cubeMapper;
        delete depthCubeMap;
    }
}