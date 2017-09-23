#include "stdafx.h"
#include "AnimationPack.h"
#include "SimpleParser.h"
#include "AnimationPlayer.h"
#include "TransformationManager.h"


AnimationPack::AnimationPack(std::string mediakey)
{
    auto parser = INIReader(mediakey);
    auto file = parser.gets("binary");
    unsigned char* bytes;
    int bytescount = Media::readBinary(file, &bytes);
    floats = (float*)bytes;
    framecount = parser.geti("framecount");

    string elements_string = parser.gets("elements");
    std::vector<string> elements = {};
    parser.splitBySpaces(elements, elements_string);

    for (int i = 0; i < elements.size(); i++) {
        string key = elements[i];
        int offset = parser.geti(key + ".offset");
        offsets[key] = offset;
    }
}


AnimationPack::~AnimationPack()
{
}

AnimationPlayer * AnimationPack::createPlayer(TransformationManager* transformation, std::string key)
{
    if (offsets.find(key) == offsets.end()) {
        printf("Element %s not found in animation pack map", key.c_str());
        return nullptr;
    }
    int offset = offsets.at(key);
    return new AnimationPlayer(transformation, floats, offset, framecount);
}
