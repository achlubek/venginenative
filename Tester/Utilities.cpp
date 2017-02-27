#include "stdafx.h"
#include "Utilities.h"
#include "lupng.h"


Utilities::Utilities()
{
}


Utilities::~Utilities()
{
}

void Utilities::convertPng16RtoUShortRawFile(string pngkey, string outname)
{
    auto a = luPngReadFile(pngkey.c_str());
    printf("%d\n", a->dataSize);
    Media::saveFile(outname, a->width*a->height*a->channels*(a->depth == 8 ? 1 : 2), a->data);
    delete a;
}
