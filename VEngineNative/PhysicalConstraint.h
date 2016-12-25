#pragma once

#include "PhysicalBody.h"
class PhysicalConstraint
{
public:
    PhysicalConstraint(btTypedConstraint* iconstraint, PhysicalBody* A, PhysicalBody* B);
    ~PhysicalConstraint();
    btTypedConstraint* constraint;
    void enable();
    void disable();
private:
    bool enabled;
    PhysicalBody* bodyA;
    PhysicalBody* bodyB;
};

