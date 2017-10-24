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
            printf("%d\n", i);
        }
        //int64_t dist = sqrt(pow(dx, 2i64) + pow(dy, 2i64) + pow(dz, 2i64));
    }
    return choosenIndex;
}

double GalaxyGenerator::drandnorm() {
    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>();
    return ((double)distr(eng)) / ((double)UINT64_MAX);
}

void GalaxyGenerator::generateStar(int64_t galaxyradius, int64_t centerThickness, double centerGravity,  uint64_t seed)
{
    SingleStar s = {};
    s.seed = seed;
    eng.seed(seed);
    s.x = randi64(-galaxyradius, galaxyradius);
    s.z = randi64(-galaxyradius, galaxyradius);
    uint64_t len = (abs(s.x) + abs(s.z)) / 2;
    double gravity = (1.0 / (1.0 + static_cast<double>(len) / 125600000.0));
   // s.x *= gravity;
  //  s.z *= gravity;

    s.y = centerThickness / (1.0 + static_cast<double>(len) /125600000.0) * (drandnorm()* 2.0 - 1.0);
    stars.push_back(s);
}

void GalaxyGenerator::addStar(SingleStar s)
{
    stars.push_back(s);
}

GeneratedStarInfo GalaxyGenerator::generateStarInfo(size_t index)
{
    auto s = stars[index];
    eng.seed(s.seed);
    GeneratedStarInfo star = {};
    star.starData = s;
    star.radius = 391000 + randu64(0, 1000000); // sun is 695700 so this range is from 391 000 to 1 391 000 pretty much covers shit
    star.color = glm::vec3(0.5 + drandnorm(), 0.5 + drandnorm(), 0.5 + drandnorm());
    star.age = drandnorm();
    star.spotsIntensity = drandnorm();
    star.rotationSpeed = drandnorm();
    star.orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
    star.planetsCount = randu64(0, 8);
    star.planets = {};
    double stardisthelper = star.radius * 10.0;
    for (int i = 0; i < star.planetsCount; i++) {
        GeneratedPlanetInfo planet = {};
        planet.radius = 3742 + randu64(0, 50000); // ranges from mercury to jupiter
        planet.terrainMaxLevel = drandnorm();
        planet.fluidMaxLevel = drandnorm() * planet.terrainMaxLevel; // no fully water planets
        planet.starDistance = stardisthelper;
        stardisthelper += planet.radius * 10.0 + randu64(0, 500000);
        planet.habitableChance = drandnorm();
        planet.orbitSpeed = drandnorm();
        planet.preferredColor = glm::vec3(0.1 + drandnorm(), 0.1 + drandnorm(), 0.1 + drandnorm());
        planet.atmosphereRadius = drandnorm() * (planet.radius * 0.05);
        planet.atmosphereAbsorbStrength = drandnorm();
        planet.atmosphereAbsorbColor = glm::vec3(0.1 + drandnorm(), 0.1 + drandnorm(), 0.1 + drandnorm());
        planet.moonsCount = randu64(0, 4);
        planet.moons = {};
        for (int g = 0; g < planet.moonsCount; g++) {
            GeneratedMoonInfo moon = {};
            moon.radius = 174 + randu64(0, 5000);
            moon.terrainMaxLevel = drandnorm();
            moon.orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
            moon.orbitSpeed = drandnorm();
            moon.planetDistance = planet.radius * 3.0 + planet.radius * drandnorm() * 3.0;
            moon.preferredColor = glm::vec3(0.1 + drandnorm(), 0.1 + drandnorm(), 0.1 + drandnorm());
            planet.moons.push_back(moon);
        }
        star.planets.push_back(planet);
    }
    return star;
}
