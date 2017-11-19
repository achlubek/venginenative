#include "stdafx.h"
#include "Player.h"
#include "AssetLoader.h"
#include "VulkanToolkit.h"


Player::Player(Object3dInfo* collisionShape) : PhysicalEntity(collisionShape, 10.0, glm::dvec3(0.0))
{
}


Player::~Player()
{
}

void Player::update(double time_delta)
{
}
