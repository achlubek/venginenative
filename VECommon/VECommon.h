#pragma once

#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/constants.hpp" // glm::pi
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "AbsTransformable.h"
#include "EventHandler.h"
#include "INIReader.h"
#include "Media.h"
#include "tinydir.h"
#include "TransformationManager.h"