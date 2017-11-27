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
    // fuck this im not that good at physics now
    // works for low velocities but for high object fly through walls (as in any other lib ;)
    /*
    for (int i = 0; i < entities.size(); i++) {
        for (int g = 0; g < entities.size(); g++) {
            if (i == g) continue;
            auto A = entities[i];
            auto B = entities[g];

            auto predictposA = A->predictPosition(time_delta);
            auto predictposB = B->predictPosition(time_delta);

            auto closestSurfaceOnA = A->closestSurface(B->getPosition());
            auto closestSurfaceOnB = B->closestSurface(A->getPosition());

            double massA = A->mass;
            double massB = B->mass;

            glm::dvec3 outposhit;
            glm::dvec3 outnormalhit;
            bool hits = A->hitRayPosition(B->getPosition(), glm::normalize(B->getLinearVelocity()), outposhit, outnormalhit);
            // only testing if B hits A, so if it hits its only B HITS A
            if (!hits || glm::distance(outposhit, B->getPosition()) > glm::distance(predictposB, B->getPosition())) {
                break;
            }
            else {
                // remember B hits A

                auto velA = A->getLinearVelocity();
                auto velB = B->getLinearVelocity();
                auto relvel = velB - velA;
                double dt = glm::abs(glm::dot(outnormalhit, glm::normalize(relvel)));
                auto reflect = glm::reflect(velB, outnormalhit);
                double massB = B->mass;
                double massA = A->mass;
                auto impulse = (massA * velA + massB * velB) / (massA + massB);
                auto impulse1 = reflect * dt;
                auto impulse2 = relvel * dt;
                B->applyAbsoluteImpulse(outposhit - B->getPosition(), outnormalhit * massB);

                // A->applyAbsoluteImpulse(outposhit - A->getPosition(), velB * dt);
                 //  A->applyAbsoluteImpulse(outposhit - B->getPosition(), outnormalhit * massB);
                  // A->applyImpulse(outposhit - A->getPosition(), impulse2 * 20.0);

                 //  double massProportion = min(massA, massB) / max(massA, massB);
                 //  auto newvelA = A->getLinearVelocity() * massProportion;
                //   auto newvelB = B->getLinearVelocity() * (1.0 - massProportion);
                  // A->applyImpulse(outposhit - A->getPosition(), massB * velB * 0.05);
                printf("col %f %f %f\n", outnormalhit.x, outnormalhit.y, outnormalhit.z);
            }
        }
    }*/
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->stepEmulation(time_delta);
    }
}
