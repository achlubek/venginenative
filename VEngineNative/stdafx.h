// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning(disable:4067)

//#define V_DEBUG 1

#include "targetver.h"

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
using namespace std;

#include "glad.h"
#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

#define glAssert() {if(glGetError() != GL_NO_ERROR) printf("ERROR ON LINE [%d] %s", __LINE__, __FILE__);}

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "stb_image.h"

#define PI 3.141592f
#define rad2deg(a) (a * (180.0f / PI))
#define deg2rad(a) (a * (PI / 180.0f))

#include "gli/gli.hpp"
#include "gli/texture.hpp"
#include "gli/texture2d.hpp"
#include "gli/convert.hpp"
#include "gli/generate_mipmaps.hpp"
#include "gli/load.hpp"

//#include "btBulletCollisionCommon.h"
//#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionShapes/btConvexShape.h"
btVector3 bulletify3(glm::vec3 v);
glm::vec3 glmify3(btVector3 v);
btQuaternion bulletifyq(glm::quat v);
glm::quat glmifyq(btQuaternion v);

#include <SFML/Audio.hpp>