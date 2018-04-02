#include "stdafx.h"
#include "UIAbsDrawable.h"
#include "UIRenderer.h"
#include "EventHandler.h"


UIAbsDrawable::UIAbsDrawable(UIRenderer* irenderer, float ix, float iy, float iwidth, float iheight, UIColor icolor)
	: renderer(irenderer), x(ix), y(iy), width(iwidth), height(iheight), color(icolor)
{
	dataBuffer = new VulkanGenericBuffer(renderer->vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1024);
    onMouseDown = EventHandler<int>();
    onMouseUp = EventHandler<int>();
}

UIAbsDrawable::~UIAbsDrawable()
{
	delete dataBuffer;
	delete set;
}

void UIAbsDrawable::draw(VulkanRenderStage * stage)
{
	if (width == 0 || height == 0) return;
	stage->setSets({ set });
	stage->drawMesh(renderer->vulkan->fullScreenQuad3dInfo, 1);
}
