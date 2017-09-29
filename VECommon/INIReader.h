#pragma once
class INIReader
{
public:
    INIReader(std::string mediakey);
    float getf(std::string key);
    glm::vec2 getv2(std::string key);
    glm::vec3 getv3(std::string key);
    void splitBySpaces(std::vector<std::string>& output, std::string src);
    int geti(std::string key);
    std::string gets(std::string key);
    ~INIReader();
private:
    std::unordered_map<std::string, std::string> data;
    void readString(std::string str);
};
