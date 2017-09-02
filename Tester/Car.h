#pragma once
#include "../VEngineNative/stdafx.h"
#include "../VEngineNative/Application.h"
#include "Media.h"
#include "../VEngineNative/Camera.h";
#include "../VEngineNative/Object3dInfo.h";
#include "../VEngineNative/Scene.h";
#include "../VEngineNative/Material.h";
#include "Sound.h";
#include "Sound3d.h";
#include "../VEngineNative/Mesh3d.h";
#include "SimpleParser.h";
#include "INIReader.h"
#include "Physics.h"
class Car
{
public:
    Car(Physics* physics, string definitionkey, TransformationManager * spawn);
    ~Car();
    Physics* physics;
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
