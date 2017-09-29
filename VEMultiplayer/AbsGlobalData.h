#pragma once
class AbsGlobalData
{
public:
    AbsGlobalData();
    ~AbsGlobalData();
    virtual std::vector<unsigned char> serialize() = 0;
    virtual void deserialize(std::vector<unsigned char> data) = 0;
};

