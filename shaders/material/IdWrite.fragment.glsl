#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

uniform uint ID;

out uint Red;


void main(){
    Red = ID;
    //gl_FragDepth = clamp(Input.Data.y, 0.0, 1.0);
}
