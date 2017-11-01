#include "stdafx.h"
#include "SpaceShipModule.h"


SpaceShipModule::SpaceShipModule(glm::dvec3 irelativePosition)
    : relativePosition (irelativePosition)
{
    enabled = false;
}


SpaceShipModule::~SpaceShipModule()
{
}

bool SpaceShipModule::isEnabled()
{
    return enabled;
}

void SpaceShipModule::enable()
{
    enabled = true;
}

void SpaceShipModule::disable()
{
    enabled = false;
}

void SpaceShipModule::setEnabled(bool value)
{
    enabled = value;
}

void SpaceShipModule::move(glm::dvec3 newRelativePos)
{
    relativePosition = newRelativePos;
}

glm::dvec3 SpaceShipModule::getRelativePosition()
{
    return relativePosition;
}
