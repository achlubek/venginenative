#pragma once
#include <random>
#include <limits>
#include <glm\glm.hpp>

struct SingleStar {
public:
    int64_t x;
    int64_t y;
    int64_t z;
    uint64_t seed;
    glm::dvec3 getPosition() {
        return glm::dvec3(x, y, z);
    }
};
struct GeneratedPlanetInfo;
struct GeneratedStarInfo;
struct GeneratedMoonInfo;

struct GeneratedStarInfo {
public:
    SingleStar starData;
    double radius; // real units like 1 391 000
    glm::dvec3 color; //0->maybe 10? maybe 100?
    double age; //0->1
    double spotsIntensity; //0->1
    double rotationSpeed; // 0-> 1
    glm::dvec3 orbitPlane; // normalized direction, normalize(cross(orbitPlane, up_vector)) == planet line
    uint8_t planetsCount;
    uint64_t starIndex;

    glm::dvec3 getPosition() {
        return glm::dvec3(starData.x, starData.y, starData.z);
    }

    glm::dvec3 getLinearVelocity() {
        return glm::dvec3(0.0);
    }

    std::string getName() {
        return "S-" + std::to_string(starIndex);
    }
};

struct GeneratedPlanetInfo {
public:
    GeneratedPlanetInfo(GeneratedStarInfo star) : host(star) {
        
    }
    GeneratedPlanetInfo() { }
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
    GeneratedStarInfo host;
    uint64_t planetIndex;

    glm::dvec3 getPosition(double timeShift) {
        double time = glfwGetTime() + timeShift;
        glm::dvec3 orbitplaneTangent = glm::normalize(glm::cross(host.orbitPlane, glm::dvec3(0.0, 1.0, 0.0)));
        return host.getPosition()
            + glm::dvec3(glm::angleAxis(orbitSpeed * time * 0.001, host.orbitPlane) * orbitplaneTangent) * starDistance;
    }

    glm::dvec3 getLinearVelocity() {
        return getPosition(1.0) - getPosition(0.0);
    }

    std::string getName() {
        return host.getName() + "-P-" + std::to_string(planetIndex);
    }
};

struct GeneratedMoonInfo {
public:
    GeneratedMoonInfo(GeneratedPlanetInfo planet) : host(planet) {

    }
    GeneratedMoonInfo() { }
    double radius; // real units idk
    double terrainMaxLevel; //0->1 
    glm::dvec3 orbitPlane;
    double orbitSpeed; //0->1
    double planetDistance; //0->1
    glm::vec3 preferredColor;//0->1
    GeneratedPlanetInfo host;
    uint64_t moonIndex;
    uint64_t planetIndex;

    glm::dvec3 getPosition(double timeShift) {
        double time = glfwGetTime() + timeShift;
        glm::dvec3 orbitplaneTangent = glm::normalize(glm::cross(orbitPlane, glm::dvec3(0.0, 1.0, 0.0)));
        return host.getPosition(timeShift)
            + glm::dvec3(glm::angleAxis(orbitSpeed * time * 0.001, glm::dvec3(orbitPlane)) * glm::dvec3(orbitplaneTangent)) * planetDistance;
    }
    glm::dvec3 getLinearVelocity() {
        return getPosition(1.0) - getPosition(0.0);
    }
    std::string getName() {
        return host.getName() + "-M-" + std::to_string(moonIndex);
    }
};

struct GeneratedStarSystemInfo {
    GeneratedStarInfo star;
    std::vector<GeneratedPlanetInfo> planets;
    std::vector<GeneratedMoonInfo> moons;

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
    size_t findClosestStar(int64_t x, int64_t y, int64_t z);
    double drandnorm();
    void generateStar(int64_t galaxyradius, int64_t centerThickness, double centerGravity, uint64_t seed);
    void addStar(SingleStar s);
    double calculateMass(double radius_km, double density = 5539.35 /*earth density in kg/m3*/) {
        double radius_meters = radius_km * 1000.0;
        double r3 = radius_meters * radius_meters * radius_meters;
        double volume = r3 * 4.1886;
        return density * volume;
    }
    double calculateGravity(double distance, float mass) {
        double G = 0.00000000006674;
        distance *= 1000.0;
        return G * (mass / max(1000.0, distance * distance));
    } 
    double calculateOrbitVelocity(double distance, float mass) {
        double G = 0.00000000006674;
        distance *= 1000.0;
        return sqrt((G * mass) / distance);
    }
    GeneratedStarSystemInfo generateStarInfo(size_t index);
private:
    std::vector<SingleStar> stars;

};

