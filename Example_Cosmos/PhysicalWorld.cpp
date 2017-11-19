#include "stdafx.h"
#include "PhysicalWorld.h"


PhysicalWorld::PhysicalWorld()
    : entities({})
{
}


PhysicalWorld::~PhysicalWorld()
{
}

void PhysicalWorld::stepEmulation(double time_delta)
{
    for (int i = 0; i < entities.size(); i++) {
        for (int g = 0; g < entities.size(); g++) {
            if (i == g) continue;
            auto A = entities[i];
            auto B = entities[g];

            auto predictposA = A->predictPosition(time_delta);
            auto predictposB = B->predictPosition(time_delta);

            auto closestSurfaceOnA = A->closestSurface(B->getPosition());
            auto closestSurfaceOnB = B->closestSurface(A->getPosition());

            glm::dvec3 outposhit;
            glm::dvec3 outnormalhit;
            bool hits = A->hitRayPosition(B->getPosition(), glm::normalize(B->getLinearVelocity()), outposhit, outnormalhit);
            if (!hits || glm::distance(outposhit, B->getPosition()) > glm::distance(B->predictPosition(time_delta), B->getPosition())) {
                //nothing, no collision
            }
            else {
                double massA = A->mass;
                double massB = B->mass;
                auto velA = A->getLinearVelocity();
                auto velB = B->getLinearVelocity();
                A->applyImpulse(outposhit - A->getPosition(), massB * velB * 0.0001);
                B->applyImpulse(outposhit - B->getPosition(), massA * velA * 0.0001);
            }
        }
    }
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->stepEmulation(time_delta);
    }
}
