#include "stdafx.h"
#include "TransformationManager.h"

using namespace glm;

TransformationManager::TransformationManager()
{
    position = vec3(0);
    size = vec3(1);
    orientation = quat();
}

TransformationManager::TransformationManager(glm::vec3 iposition)
{
    position = iposition;
    size = vec3(1);
    orientation = quat();
}

TransformationManager::TransformationManager(glm::vec3 iposition, glm::quat iorientation)
{
    position = iposition;
    size = vec3(1);
    orientation = iorientation;
}

TransformationManager::TransformationManager(glm::vec3 iposition, glm::quat iorientation, glm::vec3 isize)
{
    position = iposition;
    size = isize;
    orientation = iorientation;
}

TransformationManager::TransformationManager(glm::vec3 iposition, glm::vec3 isize)
{
    position = iposition;
    size = isize;
    orientation = quat();
}

TransformationManager::TransformationManager(glm::quat iorientation)
{
    position = vec3(0);
    size = vec3(1);
    orientation = iorientation;
}

TransformationManager::TransformationManager(glm::quat iorientation, glm::vec3 isize)
{
    position = vec3(0);
    size = isize;
    orientation = iorientation;
}

TransformationManager::~TransformationManager()
{
}

void TransformationManager::setPosition(vec3 value)
{
    position = value;
}

void TransformationManager::setSize(vec3 value)
{
    size = value;
}

void TransformationManager::setOrientation(quat value)
{
    orientation = value;
}

void TransformationManager::translate(vec3 value)
{
    position += value;
}

void TransformationManager::scale(vec3 value)
{
    size *= value;
}

void TransformationManager::rotate(quat value)
{
    orientation = value * orientation;
}

mat4 TransformationManager::getWorldTransform()
{
    mat4 rotmat = glm::mat4_cast(orientation);
    mat4 transmat = glm::translate(mat4(1), position);
    mat4 scalemat = glm::scale(mat4(1), size);
    return transmat * rotmat * scalemat;
}

mat4 TransformationManager::getInverseWorldTransform()
{
    mat4 rotmat = glm::mat4_cast(inverse(orientation));
    mat4 scalemat = glm::scale(mat4(1), 1.0f / size);
    mat4 transmat = glm::translate(mat4(1), -position);
    return scalemat * rotmat * transmat;
}

glm::mat4 TransformationManager::getRotationMatrix()
{
    return glm::mat4_cast(orientation);
}