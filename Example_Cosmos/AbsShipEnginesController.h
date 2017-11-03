#pragma once
class SpaceShip;
class AbsShipEnginesController
{
public:
    AbsShipEnginesController();
    ~AbsShipEnginesController();
    void setTargetLinearVelocity(glm::dvec3 vel);
    void setTargetAngularVelocity(glm::dvec3 vel);
    void setReferenceFrameVelocity(glm::dvec3 vel);
    void setEnginesPower(double power);
    virtual void update(SpaceShip* ship) = 0;
protected:
    glm::dvec3 referenceFrameVelocity = glm::dvec3(0.0);
    glm::dvec3 targetLinearVelocity = glm::dvec3(0.0);
    glm::dvec3 targetAngularVelocity = glm::dvec3(0.0);
    double enginesPower = 0.0;
};

