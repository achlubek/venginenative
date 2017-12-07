#include "SQLiteDatabase.h"

SQLiteDatabase::SQLiteDatabase(std::string path)
{
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        if (db != nullptr) sqlite3_close(db);
    }
}


SQLiteDatabase::~SQLiteDatabase()
{
    if(db != nullptr) sqlite3_close(db);
}

static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    std::vector<std::map<std::string, std::string>> * res = static_cast<std::vector<std::map<std::string, std::string>>*>(data);
    res->push_back({});
    for (i = 0; i<argc; i++) {
        int g = res->size() - 1;
        (*res)[g].emplace(azColName[i], argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

std::vector<std::map<std::string, std::string>> SQLiteDatabase::query(std::string sql)
{
    char *zErrMsg = 0;
    std::vector<std::map<std::string, std::string>> res = {};
    int rc = sqlite3_exec(db, sql.c_str(), callback, (void*)&res, &zErrMsg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return res;
}
