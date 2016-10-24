#pragma once
class INIReader
{
public:
    INIReader(string mediakey);
    float getf(string key);
    int geti(string key);
    string gets(string key);
    ~INIReader();
private:
    unordered_map<string, string> data;
    void readString(string str);
};

