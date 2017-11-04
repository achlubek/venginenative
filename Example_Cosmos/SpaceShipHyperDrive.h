#pragma once
#include "SpaceShipModule.h"
class SpaceShipHyperDrive : public SpaceShipModule
{
public:
    SpaceShipHyperDrive(glm::dvec3 relativePosition, glm::dvec3 thrustDirection, double power, double fuelPerSecond);
    ~SpaceShipHyperDrive();
    glm::dvec3 thrustDirection;
    double maxPower;
    double maxFuelConsumptionPerSecond;
    double currentPowerPercentage = 0.0;

    virtual void update(SpaceShip * ship, double time_elapsed) override;
    virtual double getFuelConsumptionPerSecond() override;
private:
    double realPower = 0.0;
};

