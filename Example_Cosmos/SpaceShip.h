#pragma once 
class SpaceShipModule;
class Object3dInfo;
#include "PhysicalEntity.h"

class SpaceShip : public PhysicalEntity
{
public:
    SpaceShip(Object3dInfo* info3d, glm::dvec3 pos, glm::dquat orient);
    ~SpaceShip();

    Object3dInfo * ship3dInfo;
     
    bool hitRayPosition(glm::dvec3 origin, glm::dvec3 direction, glm::dvec3 &outpos);
    glm::dvec3 closestSurface(glm::dvec3 position);

    void setHyperDriveVelocity(glm::dvec3 vel);
    std::vector<SpaceShipModule*> modules;
private:

    glm::dvec3 hyperDriveVelocity;
    
    virtual void update(double time_elapsed) override;
};

