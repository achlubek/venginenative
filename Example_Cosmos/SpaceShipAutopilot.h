#pragma once
class AbsShipEnginesController;
class SpaceShip;
class SpaceShipAutopilot
{
public:
    SpaceShipAutopilot();
    ~SpaceShipAutopilot();
    AbsShipEnginesController* controller;
    void setTargetPosition(glm::dvec3 pos);
    void getTargetOrientation(glm::dvec3 orient);
    void setAngularMaxSpeed(double val);
    void setLinearMaxSpeed(double val);
    void setPositionCorrectionStrength(double val);
    void update(SpaceShip* ship);
    void enableHyperDrive(double power);
    void disableHyperDrive();
private:
    glm::dvec3 targetPosition = glm::dvec3(0.0);
    glm::dvec3 targetOrientation = glm::dvec3(1.0, 0.0, 0.0);
    double maxAngularSpeed = 0.0;
    double maxLinearSpeed = 0.0;
    double positionCorrectionStrength = 0.0;
    double hyperDriveRealPower = 0.0;
    double hyperDriveTargetPower = 0.0;
};

