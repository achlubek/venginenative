#include "stdafx.h"
#include "AbsShipEnginesController.h"


AbsShipEnginesController::AbsShipEnginesController()
{
}


AbsShipEnginesController::~AbsShipEnginesController()
{
}

void AbsShipEnginesController::setTargetLinearVelocity(glm::dvec3 vel)
{
    targetLinearVelocity = vel;
}

void AbsShipEnginesController::setTargetAngularVelocity(glm::dvec3 vel)
{
    targetAngularVelocity = vel;
}

void AbsShipEnginesController::setReferenceFrameVelocity(glm::dvec3 vel)
{
    referenceFrameVelocity = vel;
}

void AbsShipEnginesController::setEnginesPower(double power)
{
    enginesPower = power;
}
