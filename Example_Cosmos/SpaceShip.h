#pragma once 
class SpaceShipModule;
class SpaceShip
{
public:
    SpaceShip(glm::dvec3 pos, glm::dquat orient);
    ~SpaceShip();

    glm::dvec3 getPosition();
    glm::dquat getOrientation();
    void setPosition(glm::dvec3 v);
    void setOrientation(glm::dquat r);
    glm::dvec3 getLinearVelocity();
    glm::dvec3 getAngularVelocity();
    void update(double time_elapsed);
    void applyImpulse(glm::dvec3 relativePos, glm::dvec3 force);
    void applyGravity(glm::dvec3 force);
    void setHyperDriveVelocity(glm::dvec3 vel);
    std::vector<SpaceShipModule*> modules;
private:

    glm::dvec3 position;
    glm::dquat orientation;
    glm::dvec3 linearVelocity;
    glm::dvec3 angularVelocity;
    glm::dvec3 hyperDriveVelocity;
};

