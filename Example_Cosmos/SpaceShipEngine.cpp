#include "stdafx.h"
#include "SpaceShipEngine.h"
#include "SpaceShip.h"


SpaceShipEngine::SpaceShipEngine(glm::dvec3 relativePosition, glm::dvec3 ithrustDirection, double power, double fuelPerSecond)
    : SpaceShipModule(relativePosition), 
    thrustDirection(ithrustDirection), 
    maxPower(power), 
    maxFuelConsumptionPerSecond(fuelPerSecond)
{
}


SpaceShipEngine::~SpaceShipEngine()
{
}

void SpaceShipEngine::update(SpaceShip * ship, double time_elapsed)
{
    // trust is negative of applied impulse
    glm::dvec3 force = -thrustDirection * maxPower * currentPowerPercentage;
    ship->applyImpulse(relativePosition, force);
}

double SpaceShipEngine::getFuelConsumptionPerSecond()
{
    return maxFuelConsumptionPerSecond * currentPowerPercentage;
}
