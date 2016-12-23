uniform vec2 Resolution;

#ifdef RECREATE_UV
vec2 UV = gl_FragCoord.xy / Resolution.xy;
#else
in vec2 UV;
#endif
out vec4 outColor;

layout(binding = 0) uniform sampler2D mrt_Albedo_Roughness_Tex;
layout(binding = 1) uniform sampler2D mrt_Normal_Metalness_Tex;
layout(binding = 2) uniform sampler2D mrt_Distance_Bump_Tex;

uniform mat4 VPMatrix;
uniform vec3 CameraPosition;
uniform vec3 FrustumConeLeftBottom;
uniform vec3 FrustumConeBottomLeftToBottomRight;
uniform vec3 FrustumConeBottomLeftToTopLeft;

struct PostProcessingData
{
    vec3 diffuseColor;
    vec3 normal;
    vec3 originalNormal;
    vec3 worldPos;
    vec3 cameraPos;
    float cameraDistance;
    float roughness;
    float metalness;
};

PostProcessingData currentData;

vec3 reconstructCameraSpaceDistance(vec2 uv, float dist){
    vec3 dir = normalize((FrustumConeLeftBottom + FrustumConeBottomLeftToBottomRight * uv.x + FrustumConeBottomLeftToTopLeft * uv.y));
    return dir * dist;
}

vec3 reconstructCameraSpaceAuto(vec2 uv){
    vec3 dir = normalize((FrustumConeLeftBottom + FrustumConeBottomLeftToBottomRight * uv.x + FrustumConeBottomLeftToTopLeft * uv.y));
    return dir * textureLod(mrt_Distance_Bump_Tex, uv, 0).r;
}

vec3 ToCameraSpace(vec3 position){
    return position + CameraPosition;
}

vec3 FromCameraSpace(vec3 position){
    return position - -CameraPosition;
}

float reverseLog(float dd, float far){
	//return pow(2, dd * log2(far+1.0) ) - 1;
	return pow(2, dd * log2(far)) - 1.0;
}
void createData(){
    vec4 albedo_roughness = textureLod(mrt_Albedo_Roughness_Tex, UV, 0).rgba;
    vec4 normal_metalness = textureLod(mrt_Normal_Metalness_Tex, UV, 0).rgba;
    float dist = textureLod(mrt_Distance_Bump_Tex, UV, 0).r;
    vec3 cameraSpace = reconstructCameraSpaceDistance(UV,dist);
    vec3 worldSpace = FromCameraSpace(cameraSpace);
        
    currentData = PostProcessingData(
    albedo_roughness.rgb,
    normal_metalness.rgb,
    normalize(cross(
        dFdx(worldSpace), 
        dFdy(worldSpace)
    )).xyz,
    worldSpace,
    cameraSpace,
    dist,
    albedo_roughness.a,
    normal_metalness.a
    );
}
PostProcessingData loadData(vec2 uv){
    vec4 albedo_roughness = textureLod(mrt_Albedo_Roughness_Tex, uv, 0).rgba;
    vec4 normal_metalness = textureLod(mrt_Normal_Metalness_Tex, uv, 0).rgba;
    float dist = textureLod(mrt_Distance_Bump_Tex, uv, 0).r;
    vec3 cameraSpace = reconstructCameraSpaceDistance(uv,dist);
    vec3 worldSpace = FromCameraSpace(cameraSpace);
        
    return PostProcessingData(
    albedo_roughness.rgb,
    normal_metalness.rgb,
    normalize(cross(
        dFdx(worldSpace), 
        dFdy(worldSpace)
    )).xyz,
    worldSpace,
    cameraSpace,
    dist,
    albedo_roughness.a,
    normal_metalness.a
    );
}
vec4 shade();
void main(){
    createData();
    vec4 color = shade();
    outColor = vec4(color);
}