#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

void main(){
    gl_FragDepth = clamp(Input.Data.y, 0.0, 1.0);
}
