#include "stdafx.h"
using namespace std;

INIReader::INIReader(string mediakey)
{
    readString(Media::readString(mediakey));
}

void INIReader::splitBySpaces(vector<string>& output, string src)
{
    int i = 0, d = 0;
    while (i < src.size()) {
        if (src[i] == ' ') {
            output.push_back(src.substr(d, i - d));
            d = i;
            while (src[i++] == ' ')  d++;
        }
        else {
            i++;
        }
    }
    if (i == src.size() && d < i) {
        output.push_back(src.substr(d, i));
    }
}

float INIReader::getf(string key)
{
    return atof(data[key].c_str());
}

glm::vec2 INIReader::getv2(string key)
{
    vector<string> words;
    splitBySpaces(words, data[key]);
    return glm::vec2(atof(words[0].c_str()), atof(words[1].c_str()));
}

glm::vec3 INIReader::getv3(string key)
{
    vector<string> words;
    splitBySpaces(words, data[key]);
    auto x = glm::vec3(atof(words[0].c_str()), atof(words[1].c_str()), atof(words[2].c_str()));
    return glm::vec3(atof(words[0].c_str()), atof(words[1].c_str()), atof(words[2].c_str()));
}

int INIReader::geti(string key)
{
    return atoi(data[key].c_str());
}

string INIReader::gets(string key)
{
    return data[key];
}

std::vector<std::string> INIReader::getAllKeys()
{
    return allKeys;
}

INIReader::~INIReader()
{
}

void INIReader::readString(string str)
{
    data = {};
    allKeys = {};
    stringstream sskey, ssval;
    int len = str.size();
    bool skipspaces = true;
    bool valuereading = false;
    for (int i = 0; i < len; i++) {
        if (str[i] == '\r')
            continue;
        if (str[i] == ' ' && skipspaces)
            continue;
        if (skipspaces && valuereading)
            skipspaces = false;
        if (str[i] == '\n') {
            string key = sskey.str();
            string val = ssval.str();
            sskey.str(std::string());
            ssval.str(std::string());
            if (key.size() == 0) continue;
            data[key] = val;
            allKeys.push_back(key);
            valuereading = false;
            skipspaces = true;
            continue;
        }
        if (str[i] == '=') {
            valuereading = true;
            continue;
        }
        (valuereading ? ssval : sskey) << str[i];
    }
    string key = sskey.str();
    if (key.size() != 0) {
        data[key] = ssval.str();
        allKeys.push_back(key);
    }
}