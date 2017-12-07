#pragma once
#include "sqlite3.h"
#include <string>
#include <vector>
#include <map>
class SQLiteDatabase
{
public:
    SQLiteDatabase(std::string path);
    ~SQLiteDatabase();
    std::vector<std::map<std::string, std::string>> query(std::string sql);
private:
    sqlite3 *db = nullptr;
};

