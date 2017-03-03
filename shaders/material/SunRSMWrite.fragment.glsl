#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

layout(location = 0) out vec4 Output;

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
    Output = vec4(df, distance(CameraPosition + sunDir * 1000.0, Input.WorldPos));
}
