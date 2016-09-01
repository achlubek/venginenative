#pragma once
#include "Texture2d.h";

#define NODE_MODE_ADD 0
#define NODE_MODE_MUL 1
#define NODE_MODE_AVERAGE 2
#define NODE_MODE_SUB 3
#define NODE_MODE_ALPHA 4
#define NODE_MODE_ONE_MINUS_ALPHA 5
#define NODE_MODE_REPLACE 6
#define NODE_MODE_MAX 7
#define NODE_MODE_MIN 8
#define NODE_MODE_DISTANCE 9

#define NODE_MODIFIER_ORIGINAL 0
#define NODE_MODIFIER_NEGATIVE 1
#define NODE_MODIFIER_LINEARIZE 2
#define NODE_MODIFIER_SATURATE 4
#define NODE_MODIFIER_HUE 8
#define NODE_MODIFIER_BRIGHTNESS 16
#define NODE_MODIFIER_POWER 32
#define NODE_MODIFIER_HSV 64

#define NODE_TARGET_DIFFUSE 0
#define NODE_TARGET_NORMAL 1
#define NODE_TARGET_ROUGHNESS 2
#define NODE_TARGET_METALNESS 3
#define NODE_TARGET_BUMP 4
#define NODE_TARGET_BUMP_AS_NORMAL 5
#define NODE_TARGET_DISPLACEMENT 6

#define NODE_SOURCE_COLOR 0
#define NODE_SOURCE_TEXTURE 1

#define NODE_WRAP_REPEAT 0
#define NODE_WRAP_MIRRORED 1
#define NODE_WRAP_BORDER 2

class MaterialNode
{
public:
    MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget);
    MaterialNode(Texture2d *tex, glm::vec2 uvScaling, int mixMode, int nodeTarget);

    MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier);
    MaterialNode(Texture2d *tex, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier);

    MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, float idata);
    MaterialNode(Texture2d *tex, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, float idata);

    MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, glm::vec4 idata);
    MaterialNode(Texture2d *tex, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, glm::vec4 idata);

    MaterialNode();
    ~MaterialNode();
    Texture2d *texture;
    glm::vec2 uvScale;
    glm::vec4 color;
    glm::vec4 data;
    int mixingMode;
    int target;
    int modifierflags;
    int source;
    int wrap;
};
