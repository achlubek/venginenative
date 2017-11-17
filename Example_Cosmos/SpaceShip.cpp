#include "stdafx.h"
#include "SpaceShip.h"
#include "SpaceShipModule.h"
#include "Object3dInfo.h"
#include "PhysicalEntity.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm\gtx\intersect.hpp"


SpaceShip::SpaceShip(Object3dInfo* info3d, glm::dvec3 pos, glm::dquat orient)
    : PhysicalEntity(pos, orient), modules({}), ship3dInfo(info3d)
{

}


SpaceShip::~SpaceShip()
{
}

bool triangleIntersectionDistance(glm::dvec3 origin, glm::dvec3 direction, glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 v3, double &outdistance) {
    v1 -= origin;
    v2 -= origin;
    v3 -= origin;
    origin = glm::dvec3(0.0);
    glm::dvec3 e0 = v2 - v1;
    glm::dvec3 e1 = v3 - v1;

    glm::dvec3  h = glm::cross(direction, e1);
    double a = glm::dot(e0, h);

    double f = 1.0 / a;

    glm::dvec3 s = origin - v1;
    double u = f * glm::dot(s, h);

    glm::dvec3  q = glm::cross(s, e0);
    double v = f * glm::dot(direction, q);

    double t = f * glm::dot(e1, q);

    glm::dvec3 incidentPosition = v1 + (v2 - v1)*u + (v3 - v1)*v;

    bool hits = t > 0.0 && t < 9999999.0 &&
        (a <= -0.000001 || a >= 0.000001) &&
        u >= 0.0 && u <= 1.0 &&
        v >= 0.0 && u + v <= 1.0 &&
        t >= 0.000001;
    outdistance = glm::length(incidentPosition);
    return hits;

}

glm::dvec3 closestPointOnLine(glm::dvec3 point, glm::dvec3 start, glm::dvec3 end) {
    auto a = point - start;
    auto b = end - start;
    auto len = glm::distance(a, b);
    return start + glm::clamp(glm::dot(a, b) / glm::dot(b, b), 0.0, 1.0) * b;
}

bool SpaceShip::hitRayPosition(glm::dvec3 position, glm::dvec3 direction, glm::dvec3 &outposvec)
{
    double mindist = 9999999999999.0;
    bool hit = false;
    for (int i = 0; i < ship3dInfo->vbo.size();) {
        glm::dvec3 v1 = glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v2 = glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v3 = glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        v1 = modelSpaceToWorld(v1);
        v2 = modelSpaceToWorld(v2);
        v3 = modelSpaceToWorld(v3);
        double odist = mindist;
        bool hits = triangleIntersectionDistance(position, direction, v1, v2, v3, odist);
        if (hits) {
            glm::dvec3 outpos = position + direction * odist;
            if (odist < mindist) {
                mindist = odist;
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
    // no point for vertex search because edges are tested...
    /*for (int i = 0; i < ship3dInfo->vbo.size(); i += 12) {
        glm::dvec3 v1 = glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        v1 = modelSpaceToWorld(v1);
        double dst = glm::distance(v1, position);
        if (dst < mindist) {
         //   mindist = dst;
         //   outposvec = v1;
        }
    }*/
    int tested = 0;
    for (int i = 0; i < ship3dInfo->vbo.size();) {
        glm::dvec3 v1 = glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v2 = glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v3 = glm::dvec3(ship3dInfo->vbo[i], ship3dInfo->vbo[i + 1], ship3dInfo->vbo[i + 2]);
        i += 12;
        v1 = modelSpaceToWorld(v1);
        v2 = modelSpaceToWorld(v2);
        v3 = modelSpaceToWorld(v3);
        glm::dvec3 normal = glm::normalize(glm::cross(v1 - v2, v1 - v3));
        double odist = mindist;
        bool hits = triangleIntersectionDistance(position, normal, v1, v2, v3, odist);
        if (hits) {
            glm::dvec3 outpos = position + normal * odist;
            if (odist < mindist) {
                mindist = odist;
                outposvec = outpos;
            }
        }
        normal *= -1.0;
        hits = triangleIntersectionDistance(position, normal, v1, v2, v3, odist);
        if (hits) {
            glm::dvec3 outpos = position + normal * odist;
            if (odist < mindist) {
                mindist = odist;
                outposvec = outpos;
            }
        }

        glm::dvec3 edgehit = glm::dvec3(0.0);
        edgehit = closestPointOnLine(position, v1, v2);
        odist = glm::distance(edgehit, position);
        if (odist < mindist) {
            mindist = odist;
            outposvec = edgehit;
        }        
        edgehit = closestPointOnLine(position, v2, v3);
        odist = glm::distance(edgehit, position);
        if (odist < mindist) {
            mindist = odist;
            outposvec = edgehit;
        }        
        edgehit = closestPointOnLine(position, v1, v3);
        odist = glm::distance(edgehit, position);
        if (odist < mindist) {
            mindist = odist;
            outposvec = edgehit;
        }

        tested++;
    }
    return outposvec;
}

void SpaceShip::setHyperDriveVelocity(glm::dvec3 vel)
{
    hyperDriveVelocity = vel;
}

void SpaceShip::update(double time_elapsed)
{
    for (int i = 0; i < modules.size(); i++) {
        if (modules[i]->isEnabled()) {
            modules[i]->update(this, time_elapsed);
        }
    }
}
