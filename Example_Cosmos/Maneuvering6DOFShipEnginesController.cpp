#include "stdafx.h"
#include "Maneuvering6DOFShipEnginesController.h"
#include "SpaceShipEngine.h"
#include "SpaceShip.h"


Maneuvering6DOFShipEnginesController::Maneuvering6DOFShipEnginesController()
{
}


Maneuvering6DOFShipEnginesController::~Maneuvering6DOFShipEnginesController()
{
}

void Maneuvering6DOFShipEnginesController::update(SpaceShip * ship)
{
    glm::dvec3 angularThrust = (targetAngularVelocity - ship->getAngularVelocity()) * enginesPower * 0.15;
    //X
    negYFORWARD->currentPowerPercentage = angularThrust.x;
    negYBACKWARD->currentPowerPercentage = -angularThrust.x;
    posYBACKWARD->currentPowerPercentage = angularThrust.x;
    posYFORWARD->currentPowerPercentage = -angularThrust.x;

    //Z
    negZLEFT->currentPowerPercentage = -angularThrust.y;
    negZRIGHT->currentPowerPercentage = angularThrust.y;
    posZRIGHT->currentPowerPercentage = -angularThrust.y;
    posZLEFT->currentPowerPercentage = angularThrust.y;

    //Y
    negXUP->currentPowerPercentage = angularThrust.z;
    negXDOWN->currentPowerPercentage = -angularThrust.z;
    posXDOWN->currentPowerPercentage = angularThrust.z;
    posXUP->currentPowerPercentage = -angularThrust.z;

    auto rotmat = glm::mat3_cast(ship->getOrientation());
    glm::dvec3 linearThrust = enginesPower * glm::inverse(rotmat) * ((targetLinearVelocity + referenceFrameVelocity) - ship->getLinearVelocity());

    glm::dvec3 positiveTrust = glm::dvec3(max(0.0, linearThrust.x), max(0.0, linearThrust.y), max(0.0, linearThrust.z));
    glm::dvec3 negativeTrust = -glm::dvec3(min(0.0, linearThrust.x), min(0.0, linearThrust.y), min(0.0, linearThrust.z));

    posXUP->currentPowerPercentage = glm::clamp(posXUP->currentPowerPercentage + negativeTrust.y, 0.0, 1.0);
    posXDOWN->currentPowerPercentage = glm::clamp(posXDOWN->currentPowerPercentage + positiveTrust.y, 0.0, 1.0);
    negXUP->currentPowerPercentage = glm::clamp(negXUP->currentPowerPercentage + negativeTrust.y, 0.0, 1.0);
    negXDOWN->currentPowerPercentage = glm::clamp(negXDOWN->currentPowerPercentage + positiveTrust.y, 0.0, 1.0);

    posYBACKWARD->currentPowerPercentage = glm::clamp(posYBACKWARD->currentPowerPercentage + positiveTrust.z, 0.0, 1.0);
    posYFORWARD->currentPowerPercentage = glm::clamp(posYFORWARD->currentPowerPercentage + negativeTrust.z, 0.0, 1.0);
    negYBACKWARD->currentPowerPercentage = glm::clamp(negYBACKWARD->currentPowerPercentage + positiveTrust.z, 0.0, 1.0);
    negYFORWARD->currentPowerPercentage = glm::clamp(negYFORWARD->currentPowerPercentage + negativeTrust.z, 0.0, 1.0);

    posZLEFT->currentPowerPercentage = glm::clamp(posZLEFT->currentPowerPercentage + positiveTrust.x, 0.0, 1.0);
    posZRIGHT->currentPowerPercentage = glm::clamp(posZRIGHT->currentPowerPercentage + negativeTrust.x, 0.0, 1.0);
    negZLEFT->currentPowerPercentage = glm::clamp(negZLEFT->currentPowerPercentage + positiveTrust.x, 0.0, 1.0);
    negZRIGHT->currentPowerPercentage = glm::clamp(negZRIGHT->currentPowerPercentage + negativeTrust.x, 0.0, 1.0);
}
