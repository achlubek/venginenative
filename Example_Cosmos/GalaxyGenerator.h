#pragma once
#include <random>
#include <limits>
struct SingleStar {
    int64_t x;
    int64_t y;
    int64_t z;
    uint64_t seed;
};
struct GeneratedMoonInfo {
    double radius; // real units idk
    double terrainMaxLevel; //0->1 
    glm::vec3 orbitPlane;
    double orbitSpeed; //0->1
    glm::vec3 preferredColor;//0->1
};

struct GeneratedPlanetInfo {
    double radius; // real units 12 742
    double terrainMaxLevel; //0->1
    double fluidMaxLevel; //0->1
    double starDistance; // real units like sun to earth 149 600 000
    double habitableChance; //0->1
    double orbitSpeed; //0->1
    glm::vec3 preferredColor;//0->1
    double atmosphereRadius;
    double atmosphereAbsorbStrength;//0->1
    glm::vec3 atmosphereAbsorbColor;//0->1
    uint8_t moonsCount;
    std::vector<GeneratedMoonInfo> moons;
};

struct GeneratedStarInfo {
    double radius; // real units like 1 391 000
    glm::vec3 color; //0->maybe 10? maybe 100?
    double age; //0->1
    double spotsIntensity; //0->1
    double rotationSpeed; // 0-> 1
    glm::vec3 orbitPlane; // normalized direction, normalize(cross(orbitPlane, up_vector)) == planet line
    uint8_t planetsCount;
    std::vector<GeneratedPlanetInfo> planets;
};
class GalaxyGenerator
{
public:
    GalaxyGenerator();
    ~GalaxyGenerator();
    std::random_device rd;
    std::mt19937_64 eng;
    uint64_t randu64(uint64_t min, uint64_t max);
    int64_t randi64(int64_t min, int64_t max);
    size_t findClosestStar();
    double drandnorm();
    void generateStar(int64_t minx, int64_t miny, int64_t minz, int64_t maxx, int64_t maxy, int64_t maxz, uint64_t seed);
    void addStar(SingleStar s);
    GeneratedStarInfo generateStarInfo(size_t index);
private:
    std::vector<SingleStar> stars;

};

