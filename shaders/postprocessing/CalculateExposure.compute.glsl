#version 430 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

layout(binding = 16) uniform sampler2D inputTex;

layout (std430, binding = 0) buffer R1
{
  float Luminence;
};


void main(){

    float grid[] = float[](0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9);
    float luma = 0.0;
    float lumaw = 0.0;
    float maxs = distance(vec2(0.0), vec2(0.5));
    for(int x=0;x<grid.length();x++){
        for(int y=0;y<grid.length();y++){
            vec2 coord = vec2(grid[x], grid[y]);
            vec3 c = textureLod(inputTex, coord, float(textureQueryLevels(inputTex) -2.0)).rgb;
            float w = maxs - distance(coord, vec2(0.5));
            luma += w * length(c);
            lumaw += 1.0;
        }
    }
    luma = luma;
    Luminence = clamp(mix(Luminence, luma, 0.005), 0.0, 130.0);
}
