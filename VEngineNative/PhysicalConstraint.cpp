#include "stdafx.h"
#include "PhysicalConstraint.h"
#include "Game.h"

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

void PhysicalConstraint::enable()
{
    if (enabled) return;
    Game::instance->world->physics->addConstraint(this);
    enabled = true;
}

void PhysicalConstraint::disable()
{
    if (!enabled) return;
    Game::instance->world->physics->removeConstraint(this);
    enabled = false;
}
