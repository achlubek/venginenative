#pragma once
#include "Game.h"
#include "INIReader.h"
class Car
{
public:
    Car(string definitionkey, TransformationManager * spawn);
    ~Car();
    void draw();
    void setAcceleration(float acc);
    void setWheelsAngle(float angleInRadians);
    float getWheelsAngle();
    TransformationManager* getTransformation();
    float getSpeed();
    glm::vec3 getLinearVelocity();
    volatile bool initialized = false;

    PhysicalBody* body, *tyreLF, *tyreRF, *tyreLR, *tyreRR;
    PhysicalConstraint* tyreLFCon, *tyreRFCon, *tyreLRCon, *tyreRRCon;

private:

    INIReader* definitionReader;

    static Mesh3d* bodyMesh;
    static Mesh3d* tiresMesh; //4x instanced
    void updateTyreForce(PhysicalConstraint* tyrec, bool enableMotor, float targetVelocity = 0.0f);

    void initialize(TransformStruct spawn);
    float getAirTorque();
    float wheelsAngle = 0.0f;
};

