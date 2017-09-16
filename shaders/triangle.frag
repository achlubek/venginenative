#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec4 inTangent;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in flat uint inInstanceId;
layout(location = 4) in vec3 inWorldPos;

layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outNormalsMetalness;
layout(location = 2) out float outDistance;

#include sharedBuffers.glsl

struct ModelInfo{
    mat4 Transformation;
    uvec4 idAnd4Empty;
};
layout(set = 1, binding = 0) buffer UniformBufferObject3 {
    ModelInfo ModelInfos[];
} modelData;

vec3 transform_vertex(vec3 vertex){
    ModelInfo o = modelData.ModelInfos[inInstanceId];
    return (o.Transformation * vec4(vertex, 1.0)).xyz;
}

vec3 transform_normal(vec3 normal){
    ModelInfo o = modelData.ModelInfos[inInstanceId];
    return (o.Transformation * vec4(normal, 0.0)).xyz;
}
layout(set = 2, binding = 0) uniform UniformBufferObject6 {
    vec4 RoughnessMetalness_ZeroZero;
    vec4 DiffuseColor_Zero;
    ivec4 UseTex_DNBR;
    ivec4 UseTex_M_UseSkeleton_Zero_Zero;
    vec4 ScaleTex_DN;
    vec4 ScaleTex_BR;
    vec4 ScaleTex_MZeroZero;
    uvec4 MeshId_LodLevelId_ZeroZero;
} materialData;

layout(set = 2, binding = 1) uniform sampler2D diffuseTexture;
layout(set = 2, binding = 2) uniform sampler2D normalTexture;
layout(set = 2, binding = 3) uniform sampler2D bumpTexture;
layout(set = 2, binding = 4) uniform sampler2D roughnessTexture;
layout(set = 2, binding = 5) uniform sampler2D metalnessTexture;



#define useDiffuseColorTexInt materialData.UseTex_DNBR.x
#define useNormalTexInt materialData.UseTex_DNBR.y
#define useBumpTexInt materialData.UseTex_DNBR.z
#define useRoughnessTexInt materialData.UseTex_DNBR.w
#define useMetalnessTexInt materialData.UseTex_M_UseSkeleton_Zero_Zero.x

#define DiffuseColor materialData.DiffuseColor_Zero.rgb
#define Roughness materialData.RoughnessMetalness_ZeroZero.x
#define Metalness materialData.RoughnessMetalness_ZeroZero.y

#define diffuseColorTexScale materialData.ScaleTex_DN.xy
#define normalTexScale materialData.ScaleTex_DN.zw
#define bumpTexScale materialData.ScaleTex_BR.xy
#define roughnessTexScale materialData.ScaleTex_BR.zw
#define metalnessTexScale materialData.ScaleTex_MZeroZero.xy

float bumpTexMipMap = 0.0;
float diffuseColorTexMipMap = 0.0;
float normalTexMipMap = 0.0;
#define material_adjusted_roughness_bump() (bumpTexMipMap / textureQueryLevels(bumpTex))
#define material_adjusted_roughness_normal() (bumpTexMipMap / textureQueryLevels(normalTex))

float ParallaxHeightMultiplier = 1.03;
float newParallaxHeight = 0;
float parallaxScale = 0.04 * ParallaxHeightMultiplier;
// modded http://sunandblackcat.com/tipFullView.php?topicid=28
vec2 adjustParallaxUV(vec2 uv){
    vec3 twpos = transform_normal(normalize(inTangent.xyz));
    vec3 nwpos = transform_normal(normalize(inNormal));
    vec3 bwpos =  normalize(cross(twpos, nwpos)) * inTangent.w;
    vec3 eyevec = ((CameraPosition - inWorldPos));
    vec3 V = normalize(vec3(
        dot(eyevec, twpos),
        dot(eyevec, bwpos),
        dot(eyevec, nwpos)
    ));
    vec2 T = uv;
   const float minLayers = 16;
   const float maxLayersAngle = 11;
   const float maxLayersDistance = 54;
   float numLayers = mix(minLayers, maxLayersAngle, abs(dot(nwpos, V)));
   numLayers = mix(maxLayersDistance, numLayers, clamp(distance(CameraPosition, inWorldPos) * 1, 0.0, 1.0)) * ParallaxHeightMultiplier;
   float layerHeight = 1.0 / numLayers;
   float curLayerHeight = 0;
   vec2 dtex = parallaxScale * V.xy / V.z / numLayers;
   vec2 currentTextureCoords = T;
   float heightFromTexture = 1.0 - texture(bumpTexture, currentTextureCoords).r;
   int cnt = int(numLayers);
   while(heightFromTexture > curLayerHeight && cnt-- >= 0)
   {
      curLayerHeight += layerHeight;
      currentTextureCoords -= dtex;
      heightFromTexture = 1.0 - texture(bumpTexture, currentTextureCoords).r;
   }
   vec2 prevTCoords = currentTextureCoords + dtex;
   float nextH = heightFromTexture - curLayerHeight;
   float prevH = 1.0 - texture(bumpTexture, prevTCoords).r - curLayerHeight + layerHeight;
   float weight = nextH / (nextH - prevH);
   vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0-weight);
   newParallaxHeight = curLayerHeight + prevH * weight + nextH * (1.0 - weight);
   return finalTexCoords;
}
vec3 normalxb(vec2 pos, vec2 e){
    vec3 ex = vec3(e.x, e.y, 0.0);
    vec3 a = vec3(pos.x, texture(bumpTexture, pos.xy).r * parallaxScale, pos.y);
    vec3 b = vec3(pos.x - e.x, texture(bumpTexture, pos.xy - ex.xz).r * parallaxScale, pos.y);
    vec3 c = vec3(pos.x       , texture(bumpTexture, pos.xy + ex.zy).r * parallaxScale, pos.y + e.y);
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    return normalize(normal).xyz;
}
vec3 examineBumpMap(vec2 iuv){
    return normalxb(iuv, 1.0 / (textureSize(bumpTexture, 0).xy));
}

void outputMaterial(){
    vec2 UV = inTexCoord;
    vec2 derivatives = vec2(dFdx(UV.x), dFdy(UV.y));
    vec3 orignormal = normalize(cross(dFdx(inWorldPos), dFdy(inWorldPos)));
    vec3 normal = normalize(inNormal);
    float signer = sign(dot(normal, orignormal));
    signer += 1.0 - step(0.001, abs(signer));

//
    vec3 diffuseColor = DiffuseColor;
    float metalness = Metalness;
    float roughness = Roughness;
    float normalfrequency = abs(1.0 - length(inNormal));
    vec3 normalmap = vec3(1.0);
    vec3 tangent = normalize(inTangent.rgb);

    float qr = 0.0;//textureQueryLod(normalTex, UV * normalTexScale).x / textureQueryLevels(normalTex);

    //UV += derivatives * 1.0 * vec2(rand2sTime(UV), rand2sTime(UV + 1000.0));
    float tangentSign = inTangent.w;
    vec3 worldPos = inWorldPos;
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
        diffuseColor = texture(diffuseTexture, UV * diffuseColorTexScale).rgb;
    }
    if(useNormalTexInt > 0){
        vec3 normalmaptex = normalize(texture(normalTexture, UV * normalTexScale).rgb * 2.0 - 1.0);
        qr = sqrt(abs(length(normalmaptex) - 1.0) * 111110.0);
        normalmaptex.r *= -1.0;
        //normalmaptex.g *= -1.0;
        normalmap *= normalmaptex;
        normalmap = normalize(normalmap);
    //    roughness = roughness + (1.0 - roughness) * material_adjusted_roughness_normal();
    }
    if(useRoughnessTexInt > 0){
        roughness = texture(roughnessTexture, UV * roughnessTexScale).r;
    }
    if(useMetalnessTexInt > 0){
        metalness = texture(metalnessTexture, UV * metalnessTexScale).r;
    }
    if(useNormalTexInt > 0 || useBumpTexInt > 0){
        normal = TBN * normalmap;//normalize(mix(TBN * normalmap, normal, max(material_adjusted_roughness_normal(), material_adjusted_roughness_bump())));

    }
    normal = transform_normal(normal);

    normal = normalize(normal);
    roughness = min(1.0, roughness + qr * 0.5);
    normal = mix(normal, normalize(inNormal), qr);

    outAlbedoRoughness = vec4(diffuseColor, roughness);
    outNormalsMetalness = vec4(normalize(normal), metalness);

    outDistance = max(0.01, distance(CameraPosition, worldPos));
    //gl_FragDepth = toLogDepth(outDistance, 20000.0);
}

void main() {
    vec2 UV = inTexCoord;
    diffuseColorTexMipMap = textureQueryLod(diffuseTexture, UV).y;
    bumpTexMipMap = textureQueryLod(bumpTexture, UV).y;
    normalTexMipMap = textureQueryLod(normalTexture, UV).y;
    outputMaterial();
}
