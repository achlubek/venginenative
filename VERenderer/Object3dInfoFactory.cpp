#include "stdafx.h"
#include "Object3dInfoFactory.h"
#include "Object3dInfo.h"
#include "VulkanToolkit.h"
#include "Media.h"


Object3dInfoFactory::Object3dInfoFactory(VulkanToolkit* vulkan)
    : vulkan(vulkan)
{
}


Object3dInfoFactory::~Object3dInfoFactory()
{
}

Object3dInfo * Object3dInfoFactory::build(std::string mediakey)
{
    void * cached = Media::checkCache(mediakey);
    if (cached != nullptr) {
        return (Object3dInfo*)cached;
    }
    unsigned char* bytes;
    int bytescount = Media::readBinary(mediakey, &bytes);
    float * floats = (float*)bytes;
    int floatsCount = bytescount / 4;
    std::vector<float> flo(floats, floats + floatsCount);

    auto o = build(flo);
    Media::saveCache(mediakey, o);
    return o;
}

Object3dInfo * Object3dInfoFactory::build(std::vector<float> rawData)
{
    return new Object3dInfo(vulkan->getDevice(), rawData);
}
