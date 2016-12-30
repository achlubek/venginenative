#pragma once
#include "Game.h"
class Car
{
public:
    Car();
    ~Car();
    void draw();
    void setAcceleration(float acc);
    void setWheelsAngle(float angleInRadians);

private:
    float acceleration;
    PhysicalBody* body, *tyreLF, *tyreRF, *tyreLR, *tyreRR;
    PhysicalConstraint* tyreLFCon, *tyreRFCon, *tyreLRCon, *tyreRRCon;
    Mesh3d* bodyMesh;
    Mesh3d* tiresMesh; //4x instanced

    void initialize();
    float getAirTorque();
};

