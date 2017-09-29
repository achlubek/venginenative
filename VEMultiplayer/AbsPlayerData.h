#pragma once
class AbsPlayerData
{
public:
    AbsPlayerData(unsigned int id);
    ~AbsPlayerData();
    unsigned int id;
    virtual std::vector<unsigned char> serialize() = 0;
    virtual void deserialize(std::vector<unsigned char> data) = 0;
};

