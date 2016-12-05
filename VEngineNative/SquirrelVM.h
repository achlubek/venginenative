#pragma once

#include <squirrel.h>
#include <sqconfig.h>
#include <sqstdaux.h>
#include <sqstdblob.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdsystem.h>

class SquirrelVM
{
public:
    SquirrelVM();
    ~SquirrelVM();
    void callProcedureVoid(std::string name);
    void compileFile(std::string file);
private:
    HSQUIRRELVM vm;
};
