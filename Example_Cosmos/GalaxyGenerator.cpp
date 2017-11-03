#include "stdafx.h"
#include "GalaxyGenerator.h"

#include <random>
#include <limits>

GalaxyGenerator::GalaxyGenerator()
    : stars({})
{
    eng = std::mt19937_64(rd());
}


GalaxyGenerator::~GalaxyGenerator()
{
}

uint64_t GalaxyGenerator::randu64(uint64_t min, uint64_t max) {

    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>(min, max);
    return distr(eng);
}

int64_t GalaxyGenerator::randi64(int64_t min, int64_t max) {

    std::uniform_int_distribution<int64_t> distr = std::uniform_int_distribution<int64_t>(min, max);
    return distr(eng);
}

size_t GalaxyGenerator::findClosestStar(int64_t x, int64_t y, int64_t z)
{
    size_t cnt = stars.size();
    int64_t mindist = 0;
    size_t choosenIndex = 0;
    for (int i = 0; i < cnt; i++) {
        int64_t dx = stars[i].x - x;
        int64_t dy = stars[i].y - y;
        int64_t dz = stars[i].z - z;
        int64_t brief = abs(dx) + abs(dy) + abs(dz);
        if (i == 0 || brief < mindist) {
            mindist = brief;
            choosenIndex = i;
            // printf("%d\n", i);
        }
        //int64_t dist = sqrt(pow(dx, 2i64) + pow(dy, 2i64) + pow(dz, 2i64));
    }
    return choosenIndex;
}

double GalaxyGenerator::drandnorm() {
    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>();
    return ((double)distr(eng)) / ((double)UINT64_MAX);
}
glm::dvec2 rotate(glm::dvec2 v, double a) {
    double s = sin(a);
    double c = cos(a);
    glm::dmat2 m = glm::dmat2(c, -s, s, c);
    return m * v;
}
double hashx(double n) {
    return glm::fract(sin(n)*758.5453);
}
void GalaxyGenerator::generateStar(int64_t galaxyradius, int64_t centerThickness, double centerGravity, uint64_t seed)
{
    SingleStar s = {};
    s.seed = seed;
    eng.seed(seed);
    double ss = drandnorm() * 100.0;
#define rnd() hashx(ss);ss+=100.0;

    double lu = rnd();
    double x = rnd();
    double y = rnd();
    double w = rnd();
#define drand2rn() (drandnorm() * 2.0 - 1.0)
    double a = 1.7;
    double coef1 = 1.0 / (sqrt(2.0 * 3.1415 * a * a));
    double coef2 = -1.0 / (2.0 * a * a);

    double gauss0 = coef1 * pow(2.7182818, pow(w * 1.0, 2.0) * coef2);
    glm::dvec2 c = glm::dvec2(x, y);
    c = c * 2.0 - 1.0;
    c = glm::normalize(c) *glm::dvec2(0.96, 1.0);
    c *= max(0.00, w < 0.05 ? w : (w * w * w)); // if > 0.1 then * 
    c = rotate(c, w * 13.2340);

    double dst = glm::length(c);
    double gauss1 = coef1 * pow(2.7182818, pow(dst * 2.0, 2.0) * coef2);
    double gauss2 = coef1 * pow(2.7182818, pow(drandnorm() * 2.0, 2.0) * coef2);
    // point c is in -1 -> 1
    w = 1.0 - w;
    lu = w*w*(3.0 - 2.0 * w);
    s.x = static_cast<double>(galaxyradius) * c.x;
    s.z = static_cast<double>(galaxyradius) * c.y;
    s.y = static_cast<double>(centerThickness) * (gauss1 * ((drand2rn() * drand2rn() * drand2rn() * drand2rn())));

    stars.push_back(s);
}

void GalaxyGenerator::addStar(SingleStar s)
{
    stars.push_back(s);
}

GeneratedStarSystemInfo GalaxyGenerator::generateStarInfo(size_t index)
{
    auto system = GeneratedStarSystemInfo();
    auto s = stars[index];
    eng.seed(s.seed);
    GeneratedStarInfo star = {};
    star.starData = s;
    star.radius = 39100 + randu64(0, 100000); // sun is 695700 so this range is from 391 000 to 1 391 000 pretty much covers shit
    star.color = glm::vec3(0.5 + drandnorm() * drandnorm(), 0.5 + drandnorm() * drandnorm(), 0.5 + drandnorm() * drandnorm());
    star.age = drandnorm();
    star.spotsIntensity = drandnorm();
    star.rotationSpeed = drandnorm();
    star.orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
    star.planetsCount = randu64(3, 8);
    star.starIndex = index;
    system.star = star;
    system.planets = {};
    //double stardisthelper = 5800000;
    double arra[8] = { 5800000,
        10800000,
        14900000,
        22800000,
        77800000,
        142700000,
        287100000,
        449700000 };

    for (int i = 0; i < star.planetsCount; i++) {
        GeneratedPlanetInfo planet = GeneratedPlanetInfo(star);
       // planet.host = star;
        planet.starDistance = arra[i];
        planet.planetIndex = i;
       // stardisthelper += randu64(4000000, 162600000);

        uint64_t habitableStart = 10800000;// 1082000;
        uint64_t habitableEnd = 22800000;// 3279000;
        if (planet.starDistance < habitableStart) {
            // Rocky and small ONLY
            planet.radius = randu64(2440, 5440); // ranges from mercury to roughly 2x mercury
            planet.moonsCount = randu64(1, 2);
            planet.atmosphereRadius = 0.0;
            planet.atmosphereAbsorbStrength = 0.0;
            planet.atmosphereAbsorbColor = glm::vec3(0.0);
            planet.terrainMaxLevel = drandnorm();
            planet.fluidMaxLevel = 0.0;
            planet.habitableChance = 0.0;
            planet.preferredColor = glm::vec3(0.8 + drandnorm() * 0.2, 0.4 + drandnorm() * 0.3, 0.4 + drandnorm() * 0.2);

        }
        else if (planet.starDistance >= habitableStart && planet.starDistance <= habitableEnd) {
            // earth like or venus/mars like
            planet.radius = randu64(3390, 9371); // ranges from mars to 1,5x earth
            planet.moonsCount = randu64(1, 4);
            planet.atmosphereRadius = (planet.radius * 0.02);
            planet.terrainMaxLevel = drandnorm();
            float rand1 = drandnorm();
            if (rand1 < 0.5) {
                // oxygen etc, blue marbles
                planet.atmosphereAbsorbStrength = 0.02;
                planet.atmosphereAbsorbColor = glm::vec3(0.3, 0.5, 1.0);
                planet.fluidMaxLevel = drandnorm() * 0.5 + 0.2;
                planet.habitableChance = 1.0;
                planet.preferredColor = glm::vec3(0.2 + drandnorm() * 0.2, 0.7 + drandnorm() * 0.3, 0.2 + drandnorm() * 0.2);
            }
            else {
                // some mars like
                planet.atmosphereAbsorbStrength = 0.02;
                planet.atmosphereAbsorbColor = glm::vec3(0.8 + drandnorm()* 0.2, 0.3 + drandnorm()* 0.2, 0.1 + drandnorm()* 0.1);
                planet.fluidMaxLevel = drandnorm() * 0.5;
                planet.habitableChance = 0.5;
                planet.preferredColor = glm::vec3(0.8 + drandnorm() * 0.2, 0.4 + drandnorm() * 0.3, 0.1 + drandnorm() * 0.2);
            }
        }
        else {
            // gaseous giants and small rocky shits
            float rand1 = drandnorm();
            if (rand1 < 0.8) {
                // gaseous giant
                planet.radius = randu64(25362, 69911); // ranges from uranus to jupiter
                planet.moonsCount = randu64(5, 16);
                planet.atmosphereRadius = (drandnorm() + 2.0) * (planet.radius * 0.1);
                planet.atmosphereAbsorbStrength = 0.7 + 0.3 * drandnorm();
                planet.atmosphereAbsorbColor = glm::vec3(0.1 + drandnorm(), 0.1 + drandnorm(), 0.1 + drandnorm());
                planet.terrainMaxLevel = 0.0;
                planet.fluidMaxLevel = 0.0;
                planet.habitableChance = 0.0;
                planet.preferredColor = glm::vec3(0.1 + drandnorm(), 0.1 + drandnorm(), 0.1 + drandnorm());
            }
            else {
                // rocky
                planet.radius = randu64(2440, 5440); // ranges from mercury to roughly 2x mercury
                planet.moonsCount = randu64(1, 2);
                planet.atmosphereRadius = 0.0;
                planet.atmosphereAbsorbStrength = 0.0;
                planet.atmosphereAbsorbColor = glm::vec3(0.0);
                planet.terrainMaxLevel = drandnorm();
                planet.fluidMaxLevel = 0.0;
                planet.habitableChance = 0.0;
                planet.preferredColor = glm::vec3(0.8 + drandnorm() * 0.2, 0.7 + drandnorm() * 0.3, 0.7 + drandnorm() * 0.3);
            }
        }

        planet.orbitSpeed = drandnorm();
        system.moons = {};
        for (int g = 0; g < planet.moonsCount; g++) {
            GeneratedMoonInfo moon = GeneratedMoonInfo(planet);
            moon.moonIndex = g;
           // moon.host = planet;
            moon.radius = randu64(planet.radius / 30, planet.radius / 15);
            moon.terrainMaxLevel = drandnorm();
            moon.orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
            moon.orbitSpeed = drandnorm();
            moon.planetDistance = planet.radius * 3.0 + planet.radius * drandnorm() * 3.0;
            moon.preferredColor = glm::vec3(0.6 + 0.4 * drandnorm(), 0.6 + 0.4 * drandnorm(), 0.6 + 0.4 * drandnorm());
            system.moons.push_back(moon);
        }
        system.planets.push_back(planet);
    }
    return system;
}