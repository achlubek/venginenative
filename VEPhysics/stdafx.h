#pragma once

#define WIN32_LEAN_AND_MEAN             // Wyklucz rzadko u¿ywane rzeczy z nag³ówków systemu Windows

#include <stdio.h>
#include <tchar.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <functional>
#include <thread>
#include <algorithm>
#include <queue>
#include <regex>
#include <map>
#include <unordered_map>
#include <array>
#include <fstream>
using namespace std;

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionShapes/btConvexShape.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
btVector3 bulletify3(glm::vec3 v);
glm::vec3 glmify3(btVector3 v);
btQuaternion bulletifyq(glm::quat v);
glm::quat glmifyq(btQuaternion v);

#include "VEPhysics.h"