#pragma once
#include "PhysicalEntity.h"
class Player : public PhysicalEntity
{
public:
    Player();
    ~Player();
    virtual void update(double time_delta) override;
};

