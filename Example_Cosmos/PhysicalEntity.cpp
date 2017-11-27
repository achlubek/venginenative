#include "stdafx.h"
#include "PhysicalEntity.h"
#include "Object3dInfo.h"

PhysicalEntity::PhysicalEntity(Object3dInfo * info3d, double imass, glm::dvec3 iposition, glm::dquat iorientation, glm::dvec3 ilinearVelocity, glm::dvec3 iangularVelocity)
    : collision3dInfo(info3d), mass(imass), position(iposition), orientation(iorientation), linearVelocity(ilinearVelocity), angularVelocity(iangularVelocity)
{
}

PhysicalEntity::PhysicalEntity(Object3dInfo * info3d, double imass, glm::dvec3 iposition, glm::dquat iorientation, glm::dvec3 ilinearVelocity)
    : collision3dInfo(info3d), mass(imass), position(iposition), orientation(iorientation), linearVelocity(ilinearVelocity)
{
}

PhysicalEntity::PhysicalEntity(Object3dInfo * info3d, double imass, glm::dvec3 iposition, glm::dquat iorientation)
    : collision3dInfo(info3d), mass(imass), position(iposition), orientation(iorientation)
{
}

PhysicalEntity::PhysicalEntity(Object3dInfo * info3d, double imass, glm::dvec3 iposition)
    : collision3dInfo(info3d), mass(imass), position(iposition)
{
}

PhysicalEntity::~PhysicalEntity()
{
}

glm::dvec3 PhysicalEntity::getPosition()
{
    return position;
}

glm::dquat PhysicalEntity::getOrientation()
{
    return orientation;
}

glm::dvec3 PhysicalEntity::predictPosition(double time_delta)
{
    return position + time_delta * linearVelocity;
}

glm::dquat PhysicalEntity::predictOrientation(double time_delta)
{
    return orientation
        * glm::angleAxis(time_delta * angularVelocity.x, glm::dvec3(1.0, 0.0, 0.0))
        * glm::angleAxis(time_delta * angularVelocity.y, glm::dvec3(0.0, 1.0, 0.0))
        * glm::angleAxis(time_delta * angularVelocity.z, glm::dvec3(0.0, 0.0, 1.0));
}

void PhysicalEntity::setPosition(glm::dvec3 v)
{
    position = v;
}

void PhysicalEntity::setOrientation(glm::dquat r)
{
    orientation = r;
}

glm::dvec3 PhysicalEntity::getLinearVelocity()
{
    return linearVelocity;
}

glm::dvec3 PhysicalEntity::getAngularVelocity()
{
    return angularVelocity;
}

void PhysicalEntity::setLinearVelocity(glm::dvec3 v)
{
    linearVelocity = v;
}

void PhysicalEntity::setAngularVelocity(glm::dvec3 v)
{
    angularVelocity = v;
}

void PhysicalEntity::stepEmulation(double time_delta)
{
    position = predictPosition(time_delta);
    orientation = predictOrientation(time_delta);
    update(time_delta);
}
 
void PhysicalEntity::applyImpulse(glm::dvec3 relativePos, glm::dvec3 force)
{
    if (glm::length(relativePos) > 0.0) {
        double cosine = glm::length(force) == 0 ? 0.0 : glm::dot(glm::normalize(relativePos), glm::normalize(force));
        double sine = glm::sign(cosine) * glm::sqrt(1.0 - cosine * cosine);
        double t = glm::length(relativePos) * glm::length(force) * sine;
        glm::dvec3 T = glm::cross(relativePos, force);
        angularVelocity += T / mass;
    }
    auto m3 = glm::mat3_cast(orientation);
    linearVelocity += m3 * (/*glm::abs(cosine) * */ force / mass); // 100% when relativepos == force (dot == 1) or =relativepos == force (dot == -1), 0 when dot == 0
                                                                   // it just didnt fit....
}

void PhysicalEntity::applyAbsoluteImpulse(glm::dvec3 relativePos, glm::dvec3 force)
{
    if (glm::length(relativePos) > 0.0) {
        double cosine = glm::length(force) == 0 ? 0.0 : glm::dot(glm::normalize(relativePos), glm::normalize(force));
        double sine = glm::sign(cosine) * glm::sqrt(1.0 - cosine * cosine);
        double t = glm::length(relativePos) * glm::length(force) * sine;
        glm::dvec3 T = glm::cross(relativePos, force);
        angularVelocity += T / mass;
    }
    linearVelocity += (/*glm::abs(cosine) * */ force / mass); // 100% when relativepos == force (dot == 1) or =relativepos == force (dot == -1), 0 when dot == 0
                                                                   // it just didnt fit....
}

void PhysicalEntity::applyGravity(glm::dvec3 force)
{
    linearVelocity += force;
}

glm::dvec3 PhysicalEntity::modelSpaceToWorld(glm::dvec3 v)
{
    glm::dmat3 shipmat = glm::mat3_cast(orientation);
    return shipmat * v + position;
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

bool PhysicalEntity::hitRayPosition(glm::dvec3 position, glm::dvec3 direction, glm::dvec3 &outposvec, glm::dvec3 &outnormal)
{
    double mindist = 9999999999999.0;
    bool hit = false;
    for (int i = 0; i < collision3dInfo->vbo.size();) {
        glm::dvec3 v1 = glm::dvec3(collision3dInfo->vbo[i], collision3dInfo->vbo[i + 1], collision3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v2 = glm::dvec3(collision3dInfo->vbo[i], collision3dInfo->vbo[i + 1], collision3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v3 = glm::dvec3(collision3dInfo->vbo[i], collision3dInfo->vbo[i + 1], collision3dInfo->vbo[i + 2]);
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
                glm::dvec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
                if (glm::dot(normal, direction) > 0.0) normal = -normal;
                outnormal = normal;
                hit = true;
            }
        }
    }
    return hit;
}

glm::dvec3 PhysicalEntity::closestSurface(glm::dvec3 position)
{
    glm::dvec3 outposvec = glm::dvec3(0.0);
    double mindist = 9999999999999.0;
    // no point for vertex search because edges are tested...
    /*for (int i = 0; i < collision3dInfo->vbo.size(); i += 12) {
    glm::dvec3 v1 = glm::dvec3(collision3dInfo->vbo[i], collision3dInfo->vbo[i + 1], collision3dInfo->vbo[i + 2]);
    v1 = modelSpaceToWorld(v1);
    double dst = glm::distance(v1, position);
    if (dst < mindist) {
    //   mindist = dst;
    //   outposvec = v1;
    }
    }*/
    int tested = 0;
    for (int i = 0; i < collision3dInfo->vbo.size();) {
        glm::dvec3 v1 = glm::dvec3(collision3dInfo->vbo[i], collision3dInfo->vbo[i + 1], collision3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v2 = glm::dvec3(collision3dInfo->vbo[i], collision3dInfo->vbo[i + 1], collision3dInfo->vbo[i + 2]);
        i += 12;
        glm::dvec3 v3 = glm::dvec3(collision3dInfo->vbo[i], collision3dInfo->vbo[i + 1], collision3dInfo->vbo[i + 2]);
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