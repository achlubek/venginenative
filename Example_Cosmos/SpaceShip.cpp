#include "stdafx.h"
#include "SpaceShip.h"
#include "SpaceShipModule.h"
#include "Object3dInfo.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm\gtx\intersect.hpp"


SpaceShip::SpaceShip(Object3dInfo* info3d, glm::dvec3 pos, glm::dquat orient)
    :position(pos), orientation(orient), modules({}), ship3dInfo(info3d)
{

}


SpaceShip::~SpaceShip()
{
}

bool SpaceShip::hitRayPosition(glm::dvec3 origin, glm::dvec3 direction, glm::dvec3 &outposvec)
{
    double mindist = 9999999999999.0;
    bool hit = false;
    for (int i = 0; i < ship3dInfo->vbo.size(); i+=12) {
        glm::dvec3 v1 = getPosition()
            + glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v2 = getPosition()
            + glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v3 = getPosition()
            + glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        glm::dvec3 outpos;
        bool hits = glm::intersectLineTriangle(origin, direction, v1, v2, v3, outpos);
        if (hits) {
            double dst = glm::distance(outpos, origin);
            if (dst < mindist) {
                mindist = dst;
                outposvec = outpos;
                hit = true;
            }
        }
    }
    return hit;
}

glm::dvec3 SpaceShip::closestSurface(glm::dvec3 position)
{
    glm::dvec3 outposvec = glm::dvec3(0.0);
    double mindist = 9999999999999.0; 
    for (int i = 0; i < ship3dInfo->vbo.size(); i += 12) {
        glm::dvec3 v1 = getPosition()
            + glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
         
        double dst = glm::distance(v1, position);
        if (dst < mindist) {
            mindist = dst;
            outposvec = v1;
        } 
    }
    for (int i = 0; i < ship3dInfo->vbo.size(); i += 12) {
        glm::dvec3 v1 = getPosition()
            + glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v2 = getPosition()
            + glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v3 = getPosition()
            + glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        glm::dvec3 outpos;
        glm::dvec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        bool hits = glm::intersectLineTriangle(position, normal, v1, v2, v3, outpos);
        if (hits) {
            double dst = glm::distance(outpos, position);
            if (dst < mindist) {
                mindist = dst;
                outpos = outpos;
            }
        }
        normal *= -1.0;
        hits = glm::intersectLineTriangle(position, normal, v1, v2, v3, outpos);
        if (hits) {
            double dst = glm::distance(outpos, position);
            if (dst < mindist) {
                mindist = dst;
                outpos = outpos;
            }
        }
    }
    return outposvec;
}

glm::dvec3 SpaceShip::getPosition()
{
    return position;
}

glm::dquat SpaceShip::getOrientation()
{
    return orientation;
}

void SpaceShip::setPosition(glm::dvec3 v)
{
    position = v;
}

void SpaceShip::setOrientation(glm::dquat r)
{
    orientation = r;
}

glm::dvec3 SpaceShip::getLinearVelocity()
{
    return linearVelocity;
}

glm::dvec3 SpaceShip::getAngularVelocity()
{
    return angularVelocity;
}

void SpaceShip::update(double time_elapsed)
{
    for (int i = 0; i < modules.size(); i++) {
        if (modules[i]->isEnabled()) {
            modules[i]->update(this, time_elapsed);
        }
    }
    position += time_elapsed * (linearVelocity + hyperDriveVelocity);
    orientation = orientation
        * glm::angleAxis(time_elapsed * angularVelocity.x, glm::dvec3(1.0, 0.0, 0.0))
        * glm::angleAxis(time_elapsed * angularVelocity.y, glm::dvec3(0.0, 1.0, 0.0))
        * glm::angleAxis(time_elapsed * angularVelocity.z, glm::dvec3(0.0, 0.0, 1.0));
}

void SpaceShip::applyImpulse(glm::dvec3 relativePos, glm::dvec3 force)
{
    double cosine = glm::length(force) == 0 ? 0.0 : glm::dot(glm::normalize(relativePos), glm::normalize(force));
    double sine = glm::sign(cosine) * glm::sqrt(1.0 - cosine * cosine);
    double t = glm::length(relativePos) * glm::length(force) * sine;
    glm::dvec3 T = glm::cross(relativePos, force);
    auto m3 = glm::mat3_cast(orientation);
    angularVelocity += T;
    linearVelocity += m3 * (/*glm::abs(cosine) * */ force); // 100% when relativepos == force (dot == 1) or =relativepos == force (dot == -1), 0 when dot == 0
}

void SpaceShip::applyGravity(glm::dvec3 force)
{
    linearVelocity += force;
}

void SpaceShip::setHyperDriveVelocity(glm::dvec3 vel)
{
    hyperDriveVelocity = vel;
}
