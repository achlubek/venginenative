#pragma once

class Media
{
public:
    static void loadFileMap(string path);
    static string readString(string key);
    static string getPath(string key);
    static int readBinary(string key, unsigned char** out_bytes);
    static void saveCache(string key, void* data);
    static void* checkCache(string key);

private:

    static map<string, string> mediaMap;
    static void searchRecursive(string path);
    static map<string, void*> cache;
};
