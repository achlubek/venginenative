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
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
void outputMaterial(){
    vec2 UV = Input.TexCoord;
    vec2 derivatives = vec2(dFdx(UV.x), dFdy(UV.y));
    vec3 orignormal = normalize(cross(dFdx(Input.WorldPos), dFdy(Input.WorldPos)));
    vec3 normal = normalize(Input.Normal);
    float signer = sign(dot(normal, orignormal));
    signer += 1.0 - step(0.001, abs(signer));

//
    vec3 diffuseColor = DiffuseColor;
    float metalness = Metalness;
    float roughness = Roughness;
    float normalfrequency = abs(1.0 - length(Input.Normal));
    vec3 normalmap = vec3(1.0);
    vec3 tangent = normalize(Input.Tangent.rgb);

    float qr = 0.0;//textureQueryLod(normalTex, UV * normalTexScale).x / textureQueryLevels(normalTex);

    //UV += derivatives * 1.0 * vec2(rand2sTime(UV), rand2sTime(UV + 1000.0));
    float tangentSign = Input.Tangent.w;
    vec3 worldPos = Input.WorldPos;
    mat3 TBN = (mat3(
        normalize(tangent),
        normalize(cross(normal, tangent)) * tangentSign,
        normalize(normal)
    ));
    vec3 bumpedNormal = examineBumpMap(UV * bumpTexScale).xzy;
    if(useBumpTexInt > 0){
        UV = adjustParallaxUV(UV * bumpTexScale) / bumpTexScale;
        normalmap = bumpedNormal;
        normalmap.r *= -1.0;
        normalmap.g *= -1.0;
        //roughness = roughness + (1.0 - roughness) * material_adjusted_roughness_bump();
    }
    if(useDiffuseColorTexInt > 0){
        diffuseColor = texture(diffuseColorTex, UV * diffuseColorTexScale).rgb;
    }
    if(useNormalTexInt > 0){
        vec3 normalmaptex = normalize(texture(normalTex, UV * normalTexScale).rgb * 2.0 - 1.0);
        qr = sqrt(abs(length(normalmaptex) - 1.0) * 111110.0);
        normalmaptex.r *= -1.0;
        //normalmaptex.g *= -1.0;
        normalmap *= normalmaptex;
        normalmap = normalize(normalmap);
    //    roughness = roughness + (1.0 - roughness) * material_adjusted_roughness_normal();
    }
    if(useRoughnessTexInt > 0){
        roughness = texture(roughnessTex, UV * roughnessTexScale).r;
    }
    if(useMetalnessTexInt > 0){
        metalness = texture(metalnessTex, UV * metalnessTexScale).r;
    }
    if(useNormalTexInt > 0 || useBumpTexInt > 0){
        normal = TBN * normalmap;//normalize(mix(TBN * normalmap, normal, max(material_adjusted_roughness_normal(), material_adjusted_roughness_bump())));

    }
    normal = quat_mul_vec(ModelInfos[Input.instanceId].Rotation, normal);

    normal = normalize(normal);
    roughness = min(1.0, roughness + qr * 0.5);
    normal = mix(normal, normalize(Input.Normal), qr);

    outAlbedoRoughness = vec4(diffuseColor, roughness);
    outNormalsMetalness = vec4(normalize(normal), metalness);

    outDistance = max(0.01, distance(CameraPosition, worldPos));
    //gl_FragDepth = toLogDepth(outDistance, 20000.0);
}
void main(){
    vec2 UV = Input.TexCoord;
    diffuseColorTexMipMap = textureQueryLod(diffuseColorTex, UV).y;
    bumpTexMipMap = textureQueryLod(bumpTex, UV).y;
    normalTexMipMap = textureQueryLod(normalTex, UV).y;
    outputMaterial();
}
