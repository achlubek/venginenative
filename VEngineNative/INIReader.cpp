#include "stdafx.h"
#include "INIReader.h"
#include "Media.h"


INIReader::INIReader(string mediakey)
{
    readString(Media::readString(mediakey));
}


float INIReader::getf(string key)
{
    return atof(data[key].c_str());
}

int INIReader::geti(string key)
{
    return atoi(data[key].c_str());
}

string INIReader::gets(string key)
{
    return data[key];
}

INIReader::~INIReader()
{
}

void INIReader::readString(string str)
{
    data = {};
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
    if (key.size() != 0)
        data[key] = ssval.str();
}
