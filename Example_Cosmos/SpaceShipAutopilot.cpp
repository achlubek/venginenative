#include "stdafx.h"
#include "SpaceShipAutopilot.h"
#include "AbsShipEnginesController.h"
#include "SpaceShip.h"


SpaceShipAutopilot::SpaceShipAutopilot()
{
}


SpaceShipAutopilot::~SpaceShipAutopilot()
{
}

void SpaceShipAutopilot::setTargetPosition(glm::dvec3 pos)
{
    targetPosition = pos;
}

void SpaceShipAutopilot::getTargetOrientation(glm::dvec3 orient)
{
    targetOrientation = orient;
}

void SpaceShipAutopilot::setAngularMaxSpeed(double val)
{
    maxAngularSpeed = val;
}

void SpaceShipAutopilot::setLinearMaxSpeed(double val)
{
    maxLinearSpeed = val;
}

void SpaceShipAutopilot::setPositionCorrectionStrength(double val)
{
    positionCorrectionStrength = val;
}

void SpaceShipAutopilot::update(SpaceShip * ship)
{

    glm::dvec3 shipforward = ship->getOrientation() * glm::dvec3(0.0, 0.0, -1.0);
    glm::dvec3 shipleft = ship->getOrientation() * glm::dvec3(1.0, 0.0, 0.0);
    glm::dvec3 shipup = ship->getOrientation() * glm::dvec3(0.0, 1.0, 0.0);

   // auto rotmat = glm::mat3_cast(ship->getOrientation());
    glm::dvec3 fdir = shipforward + targetOrientation * 1.1;

    float xt = glm::dot(shipup, fdir);
    float yt = glm::dot(shipleft, fdir);

    controller->setTargetAngularVelocity(maxAngularSpeed * glm::dvec3(xt, -yt, 0.0));

    auto dir = glm::normalize(targetPosition - ship->getPosition());
    double dst = glm::distance(targetPosition, ship->getPosition());
    controller->setTargetLinearVelocity(dir * glm::clamp(positionCorrectionStrength * dst, 0.0, maxLinearSpeed));
}

void SpaceShipAutopilot::enableHyperDrive(double power)
{ 
    hyperDriveTargetPower = power;
}

void SpaceShipAutopilot::disableHyperDrive()
{
    hyperDriveTargetPower = 0.0;
}
