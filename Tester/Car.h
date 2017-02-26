#pragma once
#include "Game.h"
class Car
{
public:
    Car(TransformationManager * spawn);
    ~Car();
    void draw();
    void setAcceleration(float acc);
    void setWheelsAngle(float angleInRadians);
    TransformationManager* getTransformation();
    float getSpeed();

private:
    PhysicalBody* body, *tyreLF, *tyreRF, *tyreLR, *tyreRR;
    PhysicalConstraint* tyreLFCon, *tyreRFCon, *tyreLRCon, *tyreRRCon;
    Mesh3d* bodyMesh;
    Mesh3d* tiresMesh; //4x instanced
    void updateTyreForce(PhysicalConstraint* tyrec, bool enableMotor, float targetVelocity = 0.0f);

    void initialize(TransformStruct spawn);
    volatile bool initialized = false;
    float getAirTorque();
};

