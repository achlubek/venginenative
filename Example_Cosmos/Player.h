#pragma once
#include "PhysicalEntity.h"
class Player : public PhysicalEntity
{
public:
    Player(Object3dInfo* collisionShape);
    ~Player();
    virtual void update(double time_delta) override;
};

