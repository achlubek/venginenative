#pragma once
#include "Mesh3d.h"
#include "Physics.h"
#include "Game.h"

class RagdollController
{
public:
    RagdollController(Mesh3d* m);
    ~RagdollController();
    void update();

    Mesh3d* mesh;
private:
    vector<PhysicalBody *> bonesBodies;
    vector<PhysicalConstraint *> bonesConstrs;
};

