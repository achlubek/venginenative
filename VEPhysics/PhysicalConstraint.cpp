#include "stdafx.h"
#include "PhysicalConstraint.h"

PhysicalConstraint::PhysicalConstraint(btTypedConstraint * iconstraint, PhysicalBody * A, PhysicalBody * B)
{
    constraint = iconstraint;
    bodyA = A;
    bodyB = B;
    bodyA->addConstraint(this);
    bodyB->addConstraint(this);
}

PhysicalConstraint::~PhysicalConstraint()
{
}
 