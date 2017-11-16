#pragma once 
class SpaceShipModule;
class Object3dInfo;

class SpaceShip
{
public:
    SpaceShip(Object3dInfo* info3d, glm::dvec3 pos, glm::dquat orient);
    ~SpaceShip();

    Object3dInfo * ship3dInfo;
     
    bool hitRayPosition(glm::dvec3 origin, glm::dvec3 direction, glm::dvec3 &outpos);
    glm::dvec3 closestSurface(glm::dvec3 position);

    glm::dvec3 getPosition();
    glm::dquat getOrientation();
    void setPosition(glm::dvec3 v);
    void setOrientation(glm::dquat r);
    glm::dvec3 getLinearVelocity();
    glm::dvec3 getAngularVelocity();
    void update(double time_elapsed);
    void applyImpulse(glm::dvec3 relativePos, glm::dvec3 force);
    void applyGravity(glm::dvec3 force);
    void setHyperDriveVelocity(glm::dvec3 vel);
    std::vector<SpaceShipModule*> modules;
private:

    glm::dvec3 position;
    glm::dquat orientation;
    glm::dvec3 linearVelocity;
    glm::dvec3 angularVelocity;
    glm::dvec3 hyperDriveVelocity;
};

