#pragma once
#include "PhysicalEntity.h"
class PhysicalWorld
{
public:
    PhysicalWorld();
    ~PhysicalWorld();
    std::vector<PhysicalEntity*> entities;
    void stepEmulation(double time_delta);
};

