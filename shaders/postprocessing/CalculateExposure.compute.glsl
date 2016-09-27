#version 430 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

layout(binding = 16) uniform sampler2D inputTex;

layout (std430, binding = 0) buffer R1
{
  float Luminence; 
}; 

void main(){
    vec3 c1 = textureLod(inputTex, vec2(0.5), float(textureQueryLevels(inputTex) - 2.0)).rgb;
    vec3 c2 = textureLod(inputTex, vec2(0.01, 0.5), float(textureQueryLevels(inputTex) - 2.0)).rgb;
    vec3 c3 = textureLod(inputTex, vec2(0.99, 0.5), float(textureQueryLevels(inputTex) - 2.0)).rgb;
    vec3 c = (c1 + c2 + c3) * 0.33;
    c.g *= 2.0;
    float luma = length(c);
    Luminence = clamp(mix(Luminence, luma, 0.04), 0.05, 30.0);
}