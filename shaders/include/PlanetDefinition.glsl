#ifndef _PLANET_DEFINITION_H_
#define _PLANET_DEFINITION_H_
struct Ray { vec3 o; vec3 d; };
struct Sphere { vec3 pos; float rad; };

float planetradius = 6371e3;
Sphere planet = Sphere(vec3(0), planetradius);

float minhit = 0.0;
float maxhit = 0.0;
float rsi2(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    float c = dot(oc, oc) - sphere.rad*sphere.rad;
    float disc = b * b - 4.0 * c;
    if (disc < 0.0) return -1.0;
    float q = b < 0.0 ? ((-b - sqrt(disc))/2.0) : ((-b + sqrt(disc))/2.0);
    float t0 = q;
    float t1 = c / q;
    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }
    minhit = min(t0, t1);
    maxhit = max(t0, t1);
    if (t1 < 0.0) return -1.0;
    if (t0 < 0.0) return t1;
    else return t0;
}

uniform float DayElapsed;
uniform float YearElapsed;
uniform float EquatorPoleMix;


uniform vec3 dd_sunDir;
uniform vec3 dd_moonDir;
uniform vec3 dd_sunSpaceDir;
uniform mat3 dd_viewFrame;
uniform vec3 dd_moonPos;
uniform vec3 dd_earthPos;

struct DayData{
    vec3 sunDir;
    vec3 moonDir;
    vec3 sunSpaceDir;
    mat3 viewFrame;
    vec3 moonPos;
    vec3 earthPos;
};

vec3 transformDirDays(vec3 dir, float elapsed, float yearelapsed, float equator_pole){
    dir *= rotationMatrix(vec3(1.0, 0.0, 0.0), 3.1415 * equator_pole);// move to geo coords
    dir *= rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * elapsed);// rotate around rotationaxis
    dir *= rotationMatrix(vec3(0.0, 1.0, 0.0), -6.2831 * yearelapsed);
    dir *= rotationMatrix(vec3(0.0, 0.0, 1.0), 0.4);
    return dir;
}

DayData calculateDay(float elapsed, float yearelapsed, float equator_pole){
    vec3 sunorigin = vec3(0.0);
    vec3 earthpos = sunorigin + rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * yearelapsed) * vec3(0.0, 0.0, 1.0) * 149597.870;

    //vec3 surfacepos_earthspace = normalize(mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0), equator_pole));
    //vec3 surfacepos_earthorbitspace = rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * elapsed) * surfacepos_earthspace;

    mat3 surface_frame = mat3(
        transformDirDays(vec3(1.0, 0.0, 0.0), elapsed, yearelapsed, equator_pole),
        transformDirDays(vec3(0.0, 1.0, 0.0), elapsed, yearelapsed, equator_pole),
        transformDirDays(vec3(0.0, 0.0, 1.0), elapsed, yearelapsed, equator_pole)
    );

    vec3 moonpos = earthpos + rotationMatrix(vec3(0.0, 0.0, 1.0), 6.2831 * 0.1 * yearelapsed) * rotationMatrix(vec3(0.0, 1.0, 0.0), 6.2831 * yearelapsed * 12.0) * vec3(0.0, 0.0, 1.0) * 384.402;
  // earthpos += surfacepos_earthorbitspace;

    return DayData(
        inverse(surface_frame) * normalize(sunorigin - earthpos),
        inverse(surface_frame) * normalize(moonpos - earthpos),
        -normalize(sunorigin - moonpos),
        surface_frame,
        moonpos,
        earthpos
    );
}

DayData dayData = DayData(dd_sunDir, dd_moonDir, dd_sunSpaceDir, dd_viewFrame, dd_moonPos, dd_earthPos);

#endif
