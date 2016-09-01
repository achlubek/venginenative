#include "stdafx.h"
#include "MaterialNode.h"

MaterialNode::MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget)
{
    source = NODE_SOURCE_COLOR;
    modifierflags = NODE_MODIFIER_ORIGINAL;
    color = icolor;
    data = glm::vec4(1);
    texture = nullptr;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode(Texture2d * tex, glm::vec2 uvScaling, int mixMode, int nodeTarget)
{
    source = NODE_SOURCE_TEXTURE;
    modifierflags = NODE_MODIFIER_ORIGINAL;
    color = glm::vec4(1);
    data = glm::vec4(1);
    texture = tex;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier)
{
    source = NODE_SOURCE_COLOR;
    modifierflags = modifier;
    color = icolor;
    data = glm::vec4(1);
    texture = nullptr;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode(Texture2d * tex, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier)
{
    source = NODE_SOURCE_TEXTURE;
    modifierflags = modifier;
    color = glm::vec4(1);
    data = glm::vec4(1);
    texture = tex;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, float idata)
{
    source = NODE_SOURCE_COLOR;
    modifierflags = modifier;
    color = icolor;
    data = glm::vec4(idata);
    texture = nullptr;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode(Texture2d * tex, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, float idata)
{
    source = NODE_SOURCE_TEXTURE;
    modifierflags = modifier;
    color = glm::vec4(1);
    data = glm::vec4(idata);
    texture = tex;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode(glm::vec4 icolor, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, glm::vec4 idata)
{
    source = NODE_SOURCE_COLOR;
    modifierflags = modifier;
    color = icolor;
    data = idata;
    texture = nullptr;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode(Texture2d * tex, glm::vec2 uvScaling, int mixMode, int nodeTarget, int modifier, glm::vec4 idata)
{
    source = NODE_SOURCE_TEXTURE;
    modifierflags = modifier;
    color = glm::vec4(1);
    data = idata;
    texture = tex;
    uvScale = uvScaling;
    mixingMode = mixMode;
    target = nodeTarget;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::MaterialNode()
{
    source = NODE_SOURCE_TEXTURE;
    modifierflags = NODE_MODIFIER_ORIGINAL;
    color = glm::vec4(1);
    data = glm::vec4(1);
    texture = nullptr;
    uvScale = glm::vec2(1);
    mixingMode = NODE_MODE_REPLACE;
    target = NODE_TARGET_DIFFUSE;
    wrap = NODE_WRAP_REPEAT;
}

MaterialNode::~MaterialNode()
{
    delete texture;
}