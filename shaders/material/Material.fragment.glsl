#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outNormalsMetalness;
layout(location = 2) out float outDistance;

#include Quaternions.glsl
#include ModelBuffer.glsl
#include Mesh3dUniforms.glsl

#include Material.glsl

void outputMaterial(){
    vec2 UV = Input.TexCoord;
    vec3 diffuseColor = DiffuseColor;
    float metalness = Metalness;
    float roughness = Roughness;
    float normalfrequency = abs(1.0 - length(Input.Normal));
    vec3 normal = normalize(Input.Normal);
    vec3 normalmap = vec3(1.0);
    vec3 tangent = normalize(Input.Tangent.rgb);
    float tangentSign = Input.Tangent.w;
    vec3 worldPos = Input.WorldPos;
    mat3 TBN = inverse(mat3(
        normalize(tangent),
        normalize(cross(normal, tangent)) * tangentSign,
        normalize(normal)
    ));
    /*if(useBumpTexInt > 0){
        UV = adjustParallaxUV(UV);
        normalmap = examineBumpMap(UV).xzy;
        normalmap.r *= -1.0;
        normalmap.g *= -1.0; 
    }*/
    if(useDiffuseColorTexInt > 0){
        diffuseColor = texture(diffuseColorTex, UV * diffuseColorTexScale).rgb;
    }
    if(useNormalTexInt > 0){
        vec3 normalmaptex = normalize(texture(normalTex, UV * normalTexScale).rgb * 2 - 1);
        normalmaptex.r *= -1.0;
        normalmaptex.g *= -1.0;
        normalmap *= normalmaptex;
    }
    if(useRoughnessTexInt > 0){
        roughness = texture(roughnessTex, UV * roughnessTexScale).r;
    }
    if(useMetalnessTexInt > 0){
        metalness = texture(metalnessTex, UV * metalnessTexScale).r;
    }
    if(useNormalTexInt > 0 || useBumpTexInt > 0){
        normal = TBN * normalmap;
    }
    normal = quat_mul_vec(ModelInfos[Input.instanceId].Rotation, normal);

    outAlbedoRoughness = vec4(diffuseColor, roughness);
    outNormalsMetalness = vec4(normal, metalness);

    outDistance = max(0.01, distance(CameraPosition, worldPos));
    //gl_FragDepth = toLogDepth(outDistance, 20000.0);
}
void main(){
    outputMaterial();
}
