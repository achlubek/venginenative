#pragma once
struct UIColor
{
public:
    float red{ 0 }, green{ 0 }, blue{ 0 }, alpha{ 0 };
    UIColor(float r, float g, float b, float a) : red(r), green(g), blue(b), alpha(a) { };
    ~UIColor() {};
};

