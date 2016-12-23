#include Constants.glsl
bool ignoreFalloff = false;
float CalculateFallof( float dist){
    return 1.0 / (dist * dist * 0.001 + 1.0);
}

float EnvDFGPolynomial(float green, float roughness, float ndotv )
{
    float x = pow(1.0 - roughness*0.7, 6.0);
    float y = ndotv;
 
    float b1 = -0.1688;
    float b2 = 1.895;
    float b3 = 0.9903;
    float b4 = -4.853;
    float b5 = 8.404;
    float b6 = -5.069;
    float bias = clamp( min( b1 * x + b2 * x * x, b3 + b4 * y + b5 * y * y + b6 * y * y * y ), 0.0, 1.0);
 
    float d0 = 0.6045;
    float d1 = 1.699;
    float d2 = -0.5228;
    float d3 = -3.603;
    float d4 = 1.404;
    float d5 = 0.1939;
    float d6 = 2.661;
    float delta = clamp( d0 + d1 * x + d2 * y + d3 * x * x + d4 * x * y + d5 * y * y + d6 * x * x * x, 0.0, 1.0);
    float scale = delta - bias;
 
    bias *= clamp( 50.0 * green, 0.0, 1.0);
    return scale + bias;
}

float fresnel_again(vec3 color, vec3 normal, vec3 cameraspace, float roughness){
    vec3 dir = normalize(reflect(normalize(cameraspace), normal));
    float fz = color.g;
    float base =  1.0 - abs(dot(normalize(normal), dir));
    float fresnel = (fz + (1-fz)*(pow(base, 5.0)));
    float angle = 1.0 - base;
    return fresnel;
}
float fresnel_again2x(float base, float roughness){
    float fz = roughness;
    float fresnel = (fz + (1-fz)*(pow(base, 5.0)));
    return fresnel;
}

float G1V(float dotNV, float k)
{
    return 1.0/(dotNV*(1.0-k)+k);
}

vec3 LightingFuncGGX_REF(vec3 N, vec3 V, vec3 L, float roughness, vec3 F0)
{
    float alpha = roughness*roughness;

    vec3 H = normalize(V+L);

    float dotNL = max(0.0, dot(N,L));
    float dotNV = max(0.0, dot(N,V));
    float dotNH = max(0.0, dot(N,H));
    float dotLH = max(0.0, dot(L,H));

    vec3 F;
    float D, vis;

    // D
    float alphaSqr = alpha*alpha;
    float pi = 3.14159;
    float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0;
    D = alphaSqr/(pi * denom * denom);


    float k = alpha/2.0;
    vis = G1V(dotNL,k)*G1V(dotNV,k);

    vec3 specular = dotNL * F0 * D * vis;
    return specular;
}

vec3 orenNayarDiffuse(
vec3 lightDirection,
vec3 viewDirection,
vec3 surfaceNormal,
float roughness,
vec3 albedo) {

    float LdotV = dot(lightDirection, viewDirection);
    float NdotL = dot(lightDirection, surfaceNormal);
    float NdotV = dot(surfaceNormal, viewDirection);

    float s = LdotV - NdotL * NdotV;
    float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

    float sigma2 = roughness * roughness;
    vec3 A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);

    return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

#define MaterialTypeSolid 0
#define MaterialTypeRandomlyDisplaced 1
#define MaterialTypeWater 2
#define MaterialTypeSky 3
#define MaterialTypeWetDrops 4
#define MaterialTypeGrass 5
#define MaterialTypePlanetSurface 6
#define MaterialTypeTessellatedTerrain 7
vec3 shade(
vec3 camera,
vec3 albedo, 
vec3 normal,
vec3 fragmentPosition, 
vec3 lightPosition, 
vec3 lightColor, 
float roughness, 
bool ignoreAtt
){
    vec3 lightRelativeToVPos =normalize( lightPosition - fragmentPosition);
    
    vec3 cameraRelativeToVPos = -normalize(fragmentPosition - camera);
    
    float att = CalculateFallof(distance(fragmentPosition, lightPosition));
    att = mix(1.0, att, roughness * roughness);
    
    vec3 specularComponent = LightingFuncGGX_REF(
    normal,
    cameraRelativeToVPos,
    lightRelativeToVPos,
    roughness,
    lightColor
    );
    
    
    
    return  lightColor * albedo * att;// * CalculateFallof(distance(lightPosition, fragmentPosition));
}

vec3 shadeDiffuse(
vec3 camera,
vec3 albedo, 
vec3 normal,
vec3 fragmentPosition, 
vec3 lightPosition, 
vec3 lightColor, 
float roughness, 
bool ignoreAtt
){
    vec3 lightRelativeToVPos =normalize( lightPosition - fragmentPosition);
    
    vec3 cameraRelativeToVPos = -normalize(fragmentPosition - camera);
    float att = CalculateFallof(distance(fragmentPosition, lightPosition));
    return  lightColor * orenNayarDiffuse(lightRelativeToVPos,
    cameraRelativeToVPos,
    normal,
    roughness,
    albedo) * att;
}

vec3 shadeInvariantGI(
vec3 camera,
vec3 albedo, 
vec3 fragmentPosition, 
vec3 lightPosition, 
vec3 lightColor
){
    vec3 lightRelativeToVPos =normalize( lightPosition - fragmentPosition);
    
    vec3 cameraRelativeToVPos = -normalize(fragmentPosition - camera);
    float att = CalculateFallof(distance(fragmentPosition, lightPosition));
    return albedo * lightColor * att;
}
