#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

layout(location = 0) out vec4 Output;

#include Quaternions.glsl
#include ModelBuffer.glsl
#include Mesh3dUniforms.glsl

#include Material.glsl

void main(){
    vec3 df = DiffuseColor;
    if(useDiffuseColorTexInt > 0){
        df = texture(diffuseColorTex, Input.TexCoord * diffuseColorTexScale).rgb;
    }
    vec3 normal = quat_mul_vec(ModelInfos[Input.instanceId].Rotation, normalize(Input.Normal));

    /*
    R - fragment Y pos
    G - length(albedo)
    B - dot(up, normal)
    A - 0.0 NOT SET
    */

    Output = vec4(
        Input.WorldPos.y,
        length(df),
        dot(normalize(normal), vec3(0.0, 1.0, 0.0)),
        0.0
    );
}
