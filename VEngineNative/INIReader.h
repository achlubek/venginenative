#pragma once
class INIReader
{
public:
    INIReader(string mediakey);
    float getf(string key);
    glm::vec2 getv2(string key);
    glm::vec3 getv3(string key);
    void splitBySpaces(vector<string>& output, string src);
    int geti(string key);
    string gets(string key);
    ~INIReader();
private:
    unordered_map<string, string> data;
    void readString(string str);
};
