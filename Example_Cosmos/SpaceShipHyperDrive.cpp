#include "stdafx.h"
#include "SpaceShipHyperDrive.h"

#include "SpaceShip.h"


SpaceShipHyperDrive::SpaceShipHyperDrive(glm::dvec3 relativePosition, glm::dvec3 ithrustDirection, double power, double fuelPerSecond)
    : SpaceShipModule(relativePosition),
    thrustDirection(ithrustDirection),
    maxPower(power),
    maxFuelConsumptionPerSecond(fuelPerSecond)
{
}


SpaceShipHyperDrive::~SpaceShipHyperDrive()
{
}

void SpaceShipHyperDrive::update(SpaceShip * ship, double time_elapsed)
{
    // trust is negative of applied impulse
    auto m3 = glm::mat3_cast(ship->getOrientation());
    glm::dvec3 vel = -thrustDirection * maxPower * realPower;
    if (realPower < currentPowerPercentage) {
        realPower += min(0.01 * time_elapsed, currentPowerPercentage - realPower);
    }
    else {
        realPower -= min(2.5 * time_elapsed, realPower - currentPowerPercentage);
    }
    realPower = glm::clamp(realPower, 0.0, 1.0);
    ship->setHyperDriveVelocity(m3 * vel);
}

double SpaceShipHyperDrive::getFuelConsumptionPerSecond()
{
    return maxFuelConsumptionPerSecond * currentPowerPercentage;
}
