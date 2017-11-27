#pragma once 
class SpaceShipModule;
class Object3dInfo;
#include "PhysicalEntity.h"
#include "Player.h"


class SpaceShip : public PhysicalEntity
{
public:
    SpaceShip(Object3dInfo* info3d, glm::dvec3 pos, glm::dquat orient);
    ~SpaceShip();
    
    void setHyperDriveVelocity(glm::dvec3 vel);
    std::vector<SpaceShipModule*> modules;
    Player* mainSeat = nullptr;
    glm::dvec3 mainSeatPosition = glm::dvec3(0.0, 1.0, 0.5);
    glm::dquat mainSeatOrientation = glm::dquat(1.0, 0.0, 0.0, 0.0);
private:

    glm::dvec3 hyperDriveVelocity;
    
    virtual void update(double time_elapsed) override;
};

