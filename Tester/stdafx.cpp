// stdafx.cpp : source file that includes just the standard includes
// Tester.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
btVector3 bulletify3(glm::vec3 v) {
return btVector3(v.x, v.y, v.z);
}

glm::vec3 glmify3(btVector3 v) {
	return glm::vec3(v.x(), v.y(), v.z());
}

btQuaternion bulletifyq(glm::quat v) {
	return btQuaternion(v.x, v.y, v.z, v.w);
}

glm::quat glmifyq(btQuaternion quat) {
	float X = quat.getX();
	float Y = quat.getY();
	float Z = quat.getZ();
	float W = quat.getW();
	glm::quat qt = glm::quat(X, Y, Z, W);
	qt.x = X;
	qt.y = Y;
	qt.z = Z;
	qt.w = W; // fuck you glm::quat
	return qt;
}