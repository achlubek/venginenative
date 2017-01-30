
struct ModelInfo{
    vec4 Rotation;
    vec4 Translation;
    vec4 Scale;
    uvec4 idAnd4Empty;
};

layout (std430, binding = 0) buffer MMBuffer
{
  ModelInfo ModelInfos[];
};

vec3 transform_vertex(int info, vec3 vertex){
    vec3 result = vertex;
    result *= ModelInfos[info].Scale.xyz;
    result = quat_mul_vec(ModelInfos[info].Rotation, result);
    result += ModelInfos[info].Translation.xyz;
    return result;
}

vec3 transform_normal(int info, vec3 normal){
    vec3 result = normal;
    result *= 1.0 / ModelInfos[info].Scale.xyz;
    return result;
}
