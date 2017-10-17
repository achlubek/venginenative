#include "stdafx.h"
#include "Environment.h"


Environment::Environment()
{
}


Environment::~Environment()
{
}
struct DayData {
    glm::vec3 sunDir;
    glm::vec3 moonDir;
    glm::vec3 sunSpaceDir;
    glm::mat3 viewFrame;
    glm::vec3 moonPos;
    glm::vec3 earthPos;
};
using namespace glm;
mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}

vec3 transformDirDays(vec3 dir, float elapsed, float yearelapsed, float equator_pole) {
    dir = dir *rotationMatrix(vec3(1.0, 0.0, 0.0), 3.1415 * equator_pole);// move to geo coords
    dir = dir *rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * elapsed);// rotate around rotationaxis
    dir = dir *rotationMatrix(vec3(0.0, 1.0, 0.0), -6.2831 * yearelapsed);
    dir = dir *rotationMatrix(vec3(0.0, 0.0, 1.0), 0.4);
    return dir;
}
DayData calculateDay(float elapsed, float yearelapsed, float equator_pole) {
    vec3 sunorigin = vec3(0.0);
    vec3 earthpos = sunorigin + rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * yearelapsed) * vec3(0.0, 0.0, 1.0) * 149597.870f;

    mat3 surface_frame = mat3(
        transformDirDays(vec3(1.0, 0.0, 0.0), elapsed, yearelapsed, equator_pole),
        transformDirDays(vec3(0.0, 1.0, 0.0), elapsed, yearelapsed, equator_pole),
        transformDirDays(vec3(0.0, 0.0, 1.0), elapsed, yearelapsed, equator_pole)
    );

    vec3 moonpos = earthpos + rotationMatrix(vec3(0.0, 0.0, 1.0), 6.2831 * 0.1 * yearelapsed) * rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * yearelapsed * 12.0) * vec3(0.0, 0.0, 1.0) * 384.402f;
    // earthpos += surfacepos_earthorbitspace;
    auto dd = DayData();
    dd.sunDir = inverse(surface_frame) * glm::normalize(sunorigin - earthpos);
    dd.moonDir = glm::inverse(surface_frame) * glm::normalize(moonpos - earthpos);
    dd.sunSpaceDir = -glm::normalize(sunorigin - moonpos);
    dd.viewFrame = surface_frame;
    dd.moonPos = moonpos;
    dd.earthPos = earthpos;
    return dd;
}
void Environment::updateBuffer()
{
    auto daydata = calculateDay(dayElapsed, yearElapsed, equatorPoleMix);

}
