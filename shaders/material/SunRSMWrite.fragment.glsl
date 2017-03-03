#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

layout(location = 0) out vec4 Output;
layout(location = 1) out vec4 WPosOutput;
layout(location = 2) out vec4 NormOutput;

#include Quaternions.glsl
#include ModelBuffer.glsl
#include Mesh3dUniforms.glsl

#include Material.glsl

uniform vec3 sunDir;

void main(){
    vec3 df = DiffuseColor;
    if(useDiffuseColorTexInt > 0){
        df = texture(diffuseColorTex, Input.TexCoord * diffuseColorTexScale).rgb;
    }
    Output = vec4(df, 0.0);
    WPosOutput = vec4(Input.WorldPos, 0.0);
    NormOutput = vec4(normalize(Input.Normal), 0.0);
}
