#include "stdafx.h"
#include "UIAbsDrawable.h"
#include "UIRenderer.h"
#include "EventHandler.h"


UIAbsDrawable::UIAbsDrawable(UIRenderer* irenderer, float ix, float iy, float iwidth, float iheight, UIColor icolor)
	: renderer(irenderer), x(ix), y(iy), width(iwidth), height(iheight), color(icolor)
{
    dataBuffer = renderer->getToolkit()->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, 1024);
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
	stage->drawMesh(renderer->getToolkit()->getObject3dInfoFactory()->getFullScreenQuad(), 1);
}
