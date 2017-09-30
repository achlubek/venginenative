#pragma once
class AbsPlayerData;
class AbsPlayerFactory
{
public:
    AbsPlayerFactory();
    ~AbsPlayerFactory();
    virtual AbsPlayerData* createPlayer() = 0;
    virtual size_t getPlayerDataLength() = 0;
protected:
    unsigned int lastId{ 0 };
    unsigned int nextId();
};

