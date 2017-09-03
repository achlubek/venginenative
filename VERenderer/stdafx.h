// stdafx.h: do³¹cz plik do standardowych systemowych plików do³¹czanych,
// lub specyficzne dla projektu pliki do³¹czane, które s¹ czêsto wykorzystywane, ale
// s¹ rzadko zmieniane
//

#pragma once

#include "targetver.h"

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
#include <limits>
using namespace std;

//#include "glad.h"
//#include <GLFW\glfw3.h>
//#include <GLFW\glfw3native.h>

#include "vulkan.h"
#include <GLFW/glfw3.h>
#include <memory>
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE 1

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"


#include "VECommon.h"
#include "VERenderer.h"

#define glAssert() {if(glGetError() != GL_NO_ERROR) printf("ERROR ON LINE [%d] %s", __LINE__, __FILE__);}

// TODO: W tym miejscu odwo³aj siê do dodatkowych nag³ówków wymaganych przez program
