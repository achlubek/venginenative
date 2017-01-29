#include "stdafx.h"
#include "Game.h"
#include "Mesh3dInstance.h"

Mesh3dInstance::Mesh3dInstance()
{
    initTransformation();
    id = Game::instance->getNextId();
    Game::instance->registerId(id, this);
}

Mesh3dInstance::Mesh3dInstance(TransformationManager * transmgr)
{
    transformation = transmgr;
    id = Game::instance->getNextId();
    Game::instance->registerId(id, this);
}

Mesh3dInstance::~Mesh3dInstance()
{
}