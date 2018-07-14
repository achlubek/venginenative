#include "stdafx.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#define PI 3.141592f
#define rad2deg(a) (a * (180.0f / PI))
#define deg2rad(a) (a * (PI / 180.0f))

Camera::Camera()
{
    initTransformation();
    brightness = 1.0;
    projectionMatrix = glm::mat4(1);
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
    projectionMatrix = glm::perspectiveRH(deg2rad(fov), aspectRatio, nearpl, farpl);
}

void Camera::createProjectionOrthogonal(float minx, float miny, float minz, float maxx, float maxy, float maxz)
{
    fov = 0.001f;
    updateFocalLength();
    farplane = 0.0;
    projectionMatrix = glm::orthoRH(minx, maxx, miny, maxy, minz, maxz);
}

glm::vec2 Camera::projectToScreen(glm::vec3 worldPosition)
{/*
    vec2 project(vec3 pos) {
        vec4 tmp = (ViewMatrix * vec4(pos, 1.0));
        return (tmp.xy / tmp.w) * 0.5 + 0.5;
    }*/    
    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * projectionMatrix * transformation->getInverseWorldTransform();
    glm::vec4 tmp = (vpmatrix * glm::vec4(worldPosition, 1.0f));
    float clipper = tmp.z > 0.0f ? 1.0f : 0.0f;
    return (glm::vec2(tmp.x, tmp.y) / tmp.w) * 0.5f + 0.5f - 1000.0f * (1.0f - clipper);
}
 

void Camera::updateFocalLength()
{
    focalLength = (float)(43.266f / (2.0f * tan(3.1415f * fov / 360.0f))) / 1.5f;
}