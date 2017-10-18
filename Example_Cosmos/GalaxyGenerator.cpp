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

double GalaxyGenerator::drandnorm() {
    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>();
    return ((double)distr(eng)) / ((double)UINT64_MAX);
}

void GalaxyGenerator::generateStar(int64_t minx, int64_t miny, int64_t minz, int64_t maxx, int64_t maxy, int64_t maxz, uint64_t seed)
{
    SingleStar s = {};
    s.seed = seed;
    eng.seed(seed);
    s.x = randi64(minx, maxx);
    s.y = randi64(miny, maxy);
    s.z = randi64(minz, maxz);
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
    star.radius = 391000 + randu64(0, 1000000);
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
        planet.radius = 1742 + randu64(0, 50000);
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
            moon.preferredColor = glm::vec3(0.1 + drandnorm(), 0.1 + drandnorm(), 0.1 + drandnorm());
            planet.moons.push_back(moon);
        }
        star.planets.push_back(planet);
    }
    return star;
}
