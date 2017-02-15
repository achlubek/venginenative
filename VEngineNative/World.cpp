#include "stdafx.h"
#include "Game.h"
#include "World.h"

World::World()
{
    mainDisplayCamera = nullptr;
    scene = new Scene();
    physics = new Physics();
}

World::~World()
{
    delete scene;
    delete mainDisplayCamera;
}

void World::draw(ShaderProgram *shader, Camera *camera)
{
    scene->draw();
}

void World::setUniforms(ShaderProgram * shader, Camera *camera)
{
    shader->use();
    glm::mat4 cameraViewMatrix = camera->transformation->getInverseWorldTransform();
    glm::mat4 vpmatrix = camera->projectionMatrix * cameraViewMatrix;
    glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
    glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
    camera->cone->update(inverse(rpmatrix));
    shader->setUniform("VPMatrix", vpmatrix);
    shader->setUniform("Resolution", glm::vec2(Game::instance->width, Game::instance->height));
    shader->setUniform("CameraPosition", camera->transformation->getPosition());
    shader->setUniform("MainCameraPosition", mainDisplayCamera->transformation->getPosition());
    shader->setUniform("Time", Game::instance->time);
}

void World::setSceneUniforms()
{
    scene->setUniforms(TransformStruct());
}