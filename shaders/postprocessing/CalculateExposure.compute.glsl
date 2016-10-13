#version 430 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

layout(binding = 16) uniform sampler2D inputTex;

layout (std430, binding = 0) buffer R1
{
  float Luminence; 
}; 


void main(){

    float grid[] = float[](0.5, 0.30, 0.70, 0.10, 0.90);
    float luma = 0.0;
    float lumaw = 0.0;
    for(int x=0;x<grid.length();x++){
        for(int y=0;y<grid.length();y++){
            vec2 coord = vec2(grid[x], grid[y]);
            vec3 c = textureLod(inputTex, coord, float(textureQueryLevels(inputTex) - 3.0)).rgb;
            float w = 1.4- distance(coord, vec2(0.5));
            luma += w * length(c);
            lumaw += 1.0;
        }
    }
    luma = luma / lumaw;
    Luminence = clamp(mix(Luminence, luma, 0.01), 0.0, 130.0);
}