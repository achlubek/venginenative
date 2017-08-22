#include "stdafx.h"
#include "Camera.h"
#include "Game.h"

using namespace glm;

Camera::Camera()
{
    initTransformation();
    brightness = 1.0;
    projectionMatrix = mat4(1);
    farplane = 1000.0;
    fov = 90.0f;
    updateFocalLength();
    cone = new FrustumCone();
}

Camera::~Camera()
{
}

void Camera::createProjectionPerspective(float ifov, float aspectRatio, float nearpl, float farpl)
{
    fov = ifov;
    updateFocalLength();
    farplane = farpl; 
    projectionMatrix = perspectiveRH(deg2rad(fov), aspectRatio, nearpl, farpl);
}

void Camera::makeCurrent()
{
    Game::instance->world->mainDisplayCamera = this;
}

void Camera::updateFocalLength()
{
    focalLength = (float)(43.266f / (2.0f * tan(3.1415f * fov / 360.0f))) / 1.5f;
}