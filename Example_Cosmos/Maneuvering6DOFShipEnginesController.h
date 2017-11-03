#pragma once
#include "AbsShipEnginesController.h"
class SpaceShipEngine;
class Maneuvering6DOFShipEnginesController : public AbsShipEnginesController
{
public:
    Maneuvering6DOFShipEnginesController();
    ~Maneuvering6DOFShipEnginesController();
    virtual void update(SpaceShip * ship) override;

    SpaceShipEngine* negXUP;
    SpaceShipEngine* negXDOWN;
    SpaceShipEngine* posXUP;
    SpaceShipEngine* posXDOWN;

    SpaceShipEngine* negYFORWARD;
    SpaceShipEngine* negYBACKWARD;
    SpaceShipEngine* posYFORWARD;
    SpaceShipEngine* posYBACKWARD;

    SpaceShipEngine* negZLEFT;
    SpaceShipEngine* negZRIGHT;
    SpaceShipEngine* posZLEFT;
    SpaceShipEngine* posZRIGHT;
};

