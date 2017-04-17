#version 430 core
#include PostProcessingCommonUniforms.glsl
layout(location = 0) in vec3 in_position;

struct SingleLight{
    vec4 position;
    vec4 direction_roughness;
    vec4 color;
};

layout (std430, binding = 6) buffer lightsbuffer
{
    SingleLight[] lights;
};

out vec3 rsmcolor;
out vec3 rsmposition;
out vec3 fragwpospos;
out vec3 rsmnormal;

mat3 calcLookAtMatrix(vec3 origin, vec3 target, float roll) {
  vec3 rr = vec3(sin(roll), cos(roll), 0.0);
  vec3 ww = normalize(target - origin);
  vec3 uu = normalize(cross(ww, rr));
  vec3 vv = normalize(cross(uu, ww));

  return mat3(uu, vv, ww);
}

vec3 transformIntoCone(vec3 v, float roughness){
    // -z is forward here unfortunately
    vec2 thicks = v.xy;
    float range = v.z;
    range *= 1.0 / ( roughness * roughness * 0.99 + 0.01);
    thicks *= roughness * 0.9 + 0.1;
    return vec3(thicks.x, thicks.y, range);
}

void main(){
    SingleLight L = lights[int(gl_InstanceID)];
    rsmcolor = L.color.rgb;
    rsmposition = L.position.xyz;
    float lennorm = L.position.w;
    rsmnormal = normalize(L.direction_roughness.xyz);
    float roughness = L.direction_roughness.a;
    vec3 v = in_position;
    v = transformIntoCone(v, 1.0);
    vec3 tangent = normalize(cross(rsmnormal, vec3(0.01, 1.0, 0.01)));
    vec3 bitangent = normalize(cross(tangent, rsmnormal));
    mat3 reframe = mat3(
        rsmnormal, tangent, bitangent
        );
    mat3 rotmat = calcLookAtMatrix(vec3(0.0), normalize(-rsmnormal), 0.01);
    v = rotmat * v;
    v *= 5.2 * step(0.01, lennorm);
    v += rsmposition;
    fragwpospos = v;
    gl_Position =  VPMatrix * (vec4(v, 1.0));
}
