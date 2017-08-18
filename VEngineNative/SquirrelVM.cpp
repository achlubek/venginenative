#include "stdafx.h"
#include "SquirrelVM.h"
#include "Game.h"
#include <cstdlib>
#include <stdarg.h> 
void error_handler(HSQUIRRELVM v, const SQChar * desc, const SQChar * source,
    SQInteger line, SQInteger column) {
    cout << "Squirrel Error" << endl << desc << endl << source << ":" << line;
}

#ifdef SQUNICODE

#define scvprintf vfwprintf
#else

#define scvprintf vfprintf
#endif
void printfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
    va_list vl;
    va_start(vl, s);
    scvprintf(stdout, s, vl);
    va_end(vl);
}

void errorfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
    va_list vl;
    va_start(vl, s);
    scvprintf(stderr, s, vl);
    va_end(vl);
}

#define initargs() SQInteger argvc = 2;

SQInteger sq_read_int(HSQUIRRELVM v, SQInteger idx) {
    SQInteger *i = new SQInteger[1];
    sq_getinteger(v, idx, i);
    SQInteger ret = *i;
    delete[] i;
    return ret;
}
SQFloat sq_read_float(HSQUIRRELVM v, SQInteger idx) {
    SQFloat *i = new SQFloat[1];
    sq_getfloat(v, idx, i);
    SQFloat ret = *i;
    delete[] i;
    return ret;
}
SQBool sq_read_bool(HSQUIRRELVM v, SQInteger idx) {
    SQBool *i = new SQBool[1];
    sq_getbool(v, idx, i);
    SQBool ret = *i;
    delete[] i;
    return ret;
}

char * sq_read_string(HSQUIRRELVM v, SQInteger idx) {
    SQInteger sz = sq_getsize(v, idx);
    const SQChar *fname;
    sq_getstring(v, idx, &fname);
    int len = sz;
    char* wstr = new char[len + 1];
    wcstombs(wstr, fname, len);
    wstr[len] = 0;
    //delete fname; // leak or error
    return wstr;
}

#define argint sq_read_int(v, argvc++)
#define argfloat sq_read_float(v, argvc++)
#define argbool sq_read_bool(v, argvc++)
#define argstring sq_read_string(v, argvc++)

SQInteger sqapi_game_load_scene(HSQUIRRELVM v) {
    initargs();
    char* name = argstring;
    return 1;
}

SQInteger print_args(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v); //number of arguments
    initargs();
    for (SQInteger n = 1; n <= nargs; n++)
    {
        printf("%d: ", n);
        switch (sq_gettype(v, n))
        {
        case OT_NULL:
            printf("null");
            break;
        case OT_INTEGER:
            printf("%i (integer)", argint);
            break;
        case OT_FLOAT:
            printf("%f (float)", argfloat);
            break;
        case OT_STRING:
            printf("%s (string)", argstring);
            break;
        case OT_TABLE:
            printf("table");
            break;
        case OT_ARRAY:
            printf("array");
            break;
        case OT_USERDATA:
            printf("userdata");
            break;
        case OT_CLOSURE:
            printf("closure(function)");
            break;
        case OT_NATIVECLOSURE:
            printf("native closure(C function)");
            break;
        case OT_GENERATOR:
            printf("generator");
            break;
        case OT_USERPOINTER:
            printf("userpointer");
            break;
        case OT_CLASS:
            printf("class");
            break;
        case OT_INSTANCE:
            printf("instance");
            break;
        case OT_WEAKREF:
            printf("weak reference");
            break;
        default:
            return sq_throwerror(v, L"invalid param"); //throws an exception
        }
        printf("\n");
    }
    sq_pushinteger(v, nargs); //push the number of arguments as return value
    return 1; //1 because 1 value is returned
}
SquirrelVM::SquirrelVM()
{
    vm = sq_open(1024);
    // sq_pushroottable(vm);
    // sqstd_register_iolib(vm);
    // sq_pop(vm,1);
     //sqstd_seterrorhandlers(vm);
    sq_setprintfunc(vm, printfunc, errorfunc);
    sq_setcompilererrorhandler(vm, error_handler);
    sq_pushroottable(vm);
    sq_pushstring(vm, L"echo", 4);
    sq_newclosure(vm, print_args, SQFalse); //create a new function
    sq_newslot(vm, -3, SQFalse);
    sq_pop(vm, 1); //pops the root table
}
SQInteger file_lexfeedASCII(SQUserPointer file)
{
    int ret;
    char c;
    if ((ret = fread(&c, sizeof(c), 1, (FILE *)file) > 0))
        return c;
    return 0;
}

int compile_file(HSQUIRRELVM v, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f)
    {
        int len = strlen(filename);
        wchar_t* wstr = new wchar_t[len];
        mbstowcs(wstr, filename, len);

        cout << sq_compile(v, file_lexfeedASCII, f, wstr, 1);
        sq_push(v, -2);
        cout << sq_call(v, 1, SQFalse, SQTrue);
        sq_remove(v, -1); //removes the closure

        sq_pop(v, 1); //removes the closure
        fclose(f);
        delete[] wstr;
        return 1;
    }
    return 0;
}
SquirrelVM::~SquirrelVM()
{
    sq_close(vm);
}

void SquirrelVM::callProcedureVoid(std::string name)
{
    SQInteger top = sq_gettop(vm);
    size_t len = strlen(name.c_str());
    wchar_t* wstr = new wchar_t[len];
    mbstowcs(wstr, name.c_str(), len);
    sq_pushroottable(vm);
    sq_pushstring(vm, wstr, len);
    sq_get(vm, -2); //get the function from the root table
    sq_pushroottable(vm); //'this' (function environment object)
    sq_call(vm, 1, SQFalse, SQTrue);
    sq_settop(vm, top);
    delete[] wstr;
}

void SquirrelVM::compileFile(std::string file)
{
    sq_pushroottable(vm);
    // compile_file(vm, file.c_str());
    size_t len = strlen(file.c_str());
    wchar_t* wstr = new wchar_t[len];
    mbstowcs(wstr, file.c_str(), len);
    compile_file(vm, "C:/Code/VEngineNative/media/squireeltest.txt");
    delete[] wstr;
    sq_pop(vm, 1);
}