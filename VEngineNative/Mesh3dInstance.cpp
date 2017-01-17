#include "stdafx.h"
#include "Game.h"
#include "Mesh3dInstance.h"

Mesh3dInstance::Mesh3dInstance()
{
    initTransformation();
}

Mesh3dInstance::Mesh3dInstance(TransformationManager * transmgr)
{
    transformation = transmgr;
}

Mesh3dInstance::~Mesh3dInstance()
{
}