#include "stdafx.h"

using namespace glm;


TransformStruct::TransformStruct()
{
    position = vec3(0);
    size = vec3(1);
    orientation = quat(1.0, 0.0, 0.0, 0.0);
}

TransformStruct::TransformStruct(glm::vec3 iposition)
{
    position = iposition;
    size = vec3(1);
    orientation = quat(1.0, 0.0, 0.0, 0.0);
}

TransformStruct::TransformStruct(glm::vec3 iposition, glm::quat iorientation)
{
    position = iposition;
    size = vec3(1);
    orientation = iorientation;
}

TransformStruct::TransformStruct(glm::vec3 iposition, glm::quat iorientation, glm::vec3 isize)
{
    position = iposition;
    size = isize;
    orientation = iorientation;
}

TransformStruct::TransformStruct(glm::vec3 iposition, glm::vec3 isize)
{
    position = iposition;
    size = isize;
    orientation = quat(1.0, 0.0, 0.0, 0.0);
}

TransformStruct::TransformStruct(glm::quat iorientation)
{
    position = vec3(0);
    size = vec3(1);
    orientation = iorientation;
}

TransformStruct::TransformStruct(glm::quat iorientation, glm::vec3 isize)
{
    position = vec3(0);
    size = isize;
    orientation = iorientation;
}


//##############################

TransformationManager::TransformationManager()
{
    position = vec3(0);
    size = vec3(1);
    orientation = quat(1.0, 0.0, 0.0, 0.0);
}

TransformationManager::TransformationManager(glm::vec3 iposition)
{
    position = iposition;
    size = vec3(1);
    orientation = quat(1.0, 0.0, 0.0, 0.0);
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
    orientation = quat(1.0, 0.0, 0.0, 0.0);
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

glm::vec3 TransformationManager::getPosition()
{
    return position;
}

glm::vec3 TransformationManager::getSize()
{
    return size;
}

glm::quat TransformationManager::getOrientation()
{
    return orientation;
}

void TransformationManager::setPosition(vec3 value)
{
    position = value;
    needsUpdate = true;
}

void TransformationManager::setSize(vec3 value)
{
    size = value;
    needsUpdate = true;
}

void TransformationManager::setOrientation(quat value)
{
    orientation = value;
    needsUpdate = true;
}

void TransformationManager::translate(vec3 value)
{
    position += value;
    needsUpdate = true;
}

void TransformationManager::scale(vec3 value)
{
    size *= value;
    needsUpdate = true;
}

void TransformationManager::rotate(quat value)
{
    orientation = value * orientation;
    needsUpdate = true;
}

void TransformationManager::copyFrom(TransformationManager* v)
{
    setPosition(v->getPosition());
    setOrientation(v->getOrientation());
    setSize(v->getSize());
}

void TransformationManager::copyTo(TransformationManager* v)
{
    v->setPosition(getPosition());
    v->setOrientation(getOrientation());
    v->setSize(getSize());
}

mat4 TransformationManager::getWorldTransform()
{
    if (needsUpdate) updateJoints();
    mat4 rotmat = glm::mat4_cast(inverse(orientation));
    mat4 transmat = glm::translate(mat4(1), position);
    mat4 scalemat = glm::scale(mat4(1), size);
    return transmat * rotmat * scalemat;
}

mat4 TransformationManager::getInverseWorldTransform()
{
    if (needsUpdate) updateJoints();
    mat4 rotmat = glm::mat4_cast(inverse(orientation));
    mat4 scalemat = glm::scale(mat4(1), 1.0f / size);
    mat4 transmat = glm::translate(mat4(1), -position);
    return scalemat * rotmat * transmat;
}

glm::mat4 TransformationManager::getRotationMatrix()
{
    if (needsUpdate) updateJoints();
    return glm::mat4_cast(orientation);
}

void TransformationManager::updateJoints()
{
    needsUpdate = false;
    int c = joints.size();
    for (int i = 0; i < c; i++) {
        auto j = joints[i];
        vec3 p = position;
        quat q = orientation;
        p += j->frame->position;
        q *= j->frame->orientation;
        j->target->position = p;
        j->target->orientation = q;
    }
}
