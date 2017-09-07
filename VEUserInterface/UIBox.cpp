#include "stdafx.h"
#include "UIBox.h"
#include "UIRenderer.h"


UIBox::UIBox(UIRenderer* irenderer, float ix, float iy, float iwidth, float iheight, UIColor icolor)
    : renderer(irenderer), x(ix), y(iy), width(iwidth), height(iheight), color(icolor)
{
    dataBuffer = new VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1024);
    set = renderer->layout->generateDescriptorSet();
    set.bindUniformBuffer(0, *dataBuffer);
    set.bindImageViewSampler(1, *renderer->dummyTexture);
    set.update();
}


UIBox::~UIBox()
{
}

void UIBox::updateBuffer()
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    bb.emplaceFloat32(x);
    bb.emplaceFloat32(y);
    bb.emplaceFloat32(width);
    bb.emplaceFloat32(height);

    bb.emplaceFloat32(color.red);
    bb.emplaceFloat32(color.green);
    bb.emplaceFloat32(color.blue);
    bb.emplaceFloat32(color.alpha);

    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32(0.0f);

    bb.emplaceInt32(1);
    bb.emplaceInt32(1);
    bb.emplaceInt32(1);
    bb.emplaceInt32(1);

    void* data;
    dataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    dataBuffer->unmap();
}
