#include "stdafx.h"
#include "AbsPlayerFactory.h"


AbsPlayerFactory::AbsPlayerFactory()
{
}


AbsPlayerFactory::~AbsPlayerFactory()
{
}

unsigned int AbsPlayerFactory::nextId()
{
    return lastId++;
}
