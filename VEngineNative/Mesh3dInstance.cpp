#include "stdafx.h"
#include "Application.h"
#include "Mesh3dInstance.h"

Mesh3dInstance::Mesh3dInstance()
{
    initTransformation();
    id = Application::instance->getNextId();
    Application::instance->registerId(id, this);
}

Mesh3dInstance::Mesh3dInstance(TransformationManager * transmgr)
{
    transformation = transmgr;
    id = Application::instance->getNextId();
    Application::instance->registerId(id, this);
}

Mesh3dInstance::~Mesh3dInstance()
{
}