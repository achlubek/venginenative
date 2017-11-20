#pragma once
class Object3dInfo;
class PhysicalEntity
{
public:
    PhysicalEntity(Object3dInfo * collisionInfo3d, double mass, glm::dvec3 position, glm::dquat orientation, glm::dvec3 linearVelocity, glm::dvec3 angularVelocity);
    PhysicalEntity(Object3dInfo * collisionInfo3d, double mass, glm::dvec3 position, glm::dquat orientation, glm::dvec3 linearVelocity);
    PhysicalEntity(Object3dInfo * collisionInfo3d, double mass, glm::dvec3 position, glm::dquat orientation);
    PhysicalEntity(Object3dInfo * collisionInfo3d, double mass, glm::dvec3 position);
    ~PhysicalEntity();

    double mass;
    glm::dvec3 getPosition();
    glm::dquat getOrientation();
    glm::dvec3 predictPosition(double time_delta);
    glm::dquat predictOrientation(double time_delta);
    void setPosition(glm::dvec3 v);
    void setOrientation(glm::dquat r);
    glm::dvec3 getLinearVelocity();
    glm::dvec3 getAngularVelocity();
    void setLinearVelocity(glm::dvec3 v);
    void setAngularVelocity(glm::dvec3 v);
    void stepEmulation(double time_delta);
    virtual void update(double time_delta) = 0;
    void applyImpulse(glm::dvec3 relativePos, glm::dvec3 force);
    void applyAbsoluteImpulse(glm::dvec3 relativePos, glm::dvec3 force);
    void applyGravity(glm::dvec3 force);
    glm::dvec3 modelSpaceToWorld(glm::dvec3 v);

    bool hitRayPosition(glm::dvec3 origin, glm::dvec3 direction, glm::dvec3 &outpos, glm::dvec3 &outnormal);
    glm::dvec3 closestSurface(glm::dvec3 position);
private:
    glm::dvec3 position = glm::dvec3(0.0);
    glm::dquat orientation = glm::dquat(1.0, 0.0, 0.0, 0.0);
    glm::dvec3 linearVelocity = glm::dvec3(0.0);
    glm::dvec3 angularVelocity = glm::dvec3(0.0);
    Object3dInfo * collision3dInfo;
};

