#pragma once
class SpaceShip;
class SpaceShipModule
{
public:
    SpaceShipModule(glm::dvec3 relativePosition);
    ~SpaceShipModule();
    virtual double getFuelConsumptionPerSecond() = 0;
    virtual void update(SpaceShip* ship, double time_elapsed) = 0;
    bool isEnabled();
    void enable();
    void disable();
    void setEnabled(bool value);
    void move(glm::dvec3 newRelativePos);
    glm::dvec3 getRelativePosition();
protected:
    glm::dvec3 relativePosition; 
    bool enabled;
};

