#pragma once
#include "UIColor.h"
#include "EventHandler.h"
class UIRenderer;
class UIAbsDrawable
{
public:
	UIAbsDrawable(UIRenderer* renderer, float x, float y, float width, float height, UIColor icolor);
	virtual ~UIAbsDrawable();
	float x{ 0 }, y{ 0 }, width{ 0 }, height{ 0 };
	UIColor color{ UIColor(0.0, 0.0, 0.0, 0.0) };
	UIRenderer* renderer{ nullptr };
	VulkanDescriptorSet* set;
	VulkanGenericBuffer* dataBuffer{ nullptr };
	int32_t zIndex{ 0 };
	virtual void updateBuffer() = 0;
	void draw(VulkanRenderStage* stage);
	enum class HorizontalAlignment { left, center, right };
	enum class VerticalAlignment { top, center, bottom };
	HorizontalAlignment horizontalAlignment = HorizontalAlignment::left;
	VerticalAlignment verticalAlignment = VerticalAlignment::top;
    EventHandler<int> onMouseDown;
    EventHandler<int> onMouseUp;
};

