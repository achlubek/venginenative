#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

//out float Depth;

#include Quaternions.glsl
#include ModelBuffer.glsl
#include Mesh3dUniforms.glsl

#include Material.glsl

uniform float CutOffDistance;
uniform float CSMRadius;

//out float Red;

float toLogDepth(float depth, float far){
    float badass_depth = log2(max(1e-6, 1.0 + depth)) / (log2(far));
    return badass_depth;
}

void main(){
    //float bump = getBump(Input.TexCoord);
    //if(Input.Data.x < 1.0 && bump >= Input.Data.x) discard;
  //  gl_FragDepth = Input.Data.y;
    gl_FragDepth = clamp(Input.Data.y, 0.0, 1.0);
  // Red = Input.Data.y;
}