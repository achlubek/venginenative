#include "stdafx.h"
#include "SpotLight.h"


SpotLight::SpotLight()
{
    initTransformation();
    id = Application::instance->getNextId();
    Application::instance->registerId(id, this);
}

SpotLight::SpotLight(TransformationManager *transmgr)
{
    transformation = transmgr;
    id = Application::instance->getNextId();
    Application::instance->registerId(id, this);
}

SpotLight::~SpotLight()
{
}
