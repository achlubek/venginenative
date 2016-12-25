// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning(disable:4067)

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <unordered_set>
#include <functional>
#include <thread>
#include <algorithm>
#include <queue>
#include <regex>

using namespace std;

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Media.h";

#include "Game.h";

#include "gli/gli.hpp"
#include "gli/texture.hpp"
#include "gli/texture2d.hpp"
#include "gli/convert.hpp"
#include "gli/generate_mipmaps.hpp"
#include "gli/load.hpp"

#define PI 3.141592f
#define rad2deg(a) (a * (180.0f / PI))
#define deg2rad(a) (a * (PI / 180.0f))

// TODO: reference additional headers your program requires here

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#ifndef bullettools
#define bullettools

btVector3 bulletify3(glm::vec3 v) {
    return btVector3(v.x, v.y, v.z);
}

glm::vec3 glmify3(btVector3 v) {
    return glm::vec3(v.x(), v.y(), v.z());
}

btQuaternion bulletifyq(glm::quat v) {
    return btQuaternion(v.x, v.y, v.z, v.w);
}

glm::quat glmifyq(btQuaternion v) {
    return glm::quat(v.x(), v.y(), v.z(), v.w());
}

#endif