#include "stdafx.h"
#include "UIBitmap.h"
#include "UIRenderer.h"


UIBitmap::UIBitmap(UIRenderer* irenderer, float ix, float iy, float iwidth, float iheight, VulkanImage* itexture, UIColor icolor)
    : renderer(irenderer), x(ix), y(iy), width(iwidth), height(iheight), texture(itexture), color(icolor)
{
    dataBuffer = new VulkanGenericBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1024);
    set = renderer->layout->generateDescriptorSet();
    set.bindUniformBuffer(0, dataBuffer);
    set.bindImageViewSampler(1, texture);
    set.update();
}


UIBitmap::~UIBitmap()
{
}

void UIBitmap::updateBuffer()
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
    bb.emplaceFloat32(1.0f);
    bb.emplaceFloat32(1.0f);

    bb.emplaceInt32(2);
    bb.emplaceInt32(2);
    bb.emplaceInt32(2);
    bb.emplaceInt32(2);

    void* data;
    dataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    dataBuffer->unmap();
}
