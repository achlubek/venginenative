#include "stdafx.h"
#include "SpaceShip.h"
#include "SpaceShipModule.h"
#include "Object3dInfo.h"
#include "PhysicalEntity.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm\gtx\intersect.hpp"


SpaceShip::SpaceShip(Object3dInfo* info3d, glm::dvec3 pos, glm::dquat orient)
    : PhysicalEntity(info3d, 1000.0, pos, orient), modules({})
{

}


SpaceShip::~SpaceShip()
{
}


void SpaceShip::setHyperDriveVelocity(glm::dvec3 vel)
{
    hyperDriveVelocity = vel;
}

void SpaceShip::update(double time_elapsed)
{
    for (int i = 0; i < modules.size(); i++) {
        if (modules[i]->isEnabled()) {
            modules[i]->update(this, time_elapsed);
        }
    }
    if (mainSeat != nullptr) {
        mainSeat->setPosition(getPosition() + getOrientation() * mainSeatPosition);
        mainSeat->setOrientation(mainSeatOrientation * getOrientation());
        mainSeat->setLinearVelocity(getLinearVelocity());
        mainSeat->setAngularVelocity(getAngularVelocity());
    }
}
