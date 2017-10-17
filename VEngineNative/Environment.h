#pragma once
class Environment
{
public:
    Environment();
    ~Environment();

    void updateBuffer();

private:
    float rainIntensity;
    float cumulusLowThreshold;
    float cumulusHighThreshold;
    float cirrusLowThreshold;
    float cirrusHighThreshold;
    float dayElapsed;
    float yearElapsed;
    float equatorPoleMix;
};

