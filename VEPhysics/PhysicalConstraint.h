#pragma once
#include "PhysicalBody.h"
class btTypedConstraint;
class PhysicalConstraint
{
public:
    PhysicalConstraint(btTypedConstraint* iconstraint, PhysicalBody* A, PhysicalBody* B);
    ~PhysicalConstraint();
    btTypedConstraint* constraint; 
private:
    PhysicalBody* bodyA;
    PhysicalBody* bodyB;
};

