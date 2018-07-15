#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(set = 0, binding = 0) uniform ubo {
    float Time;
} buff;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out float outDepth;

mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

void main() {
    mat3 rotmat = rotationMatrix(vec3(0,1,0), buff.Time);
    rotmat = rotmat * rotationMatrix(vec3(1,0,0), buff.Time*0.1);
    vec3 pos = rotmat * inPosition.xyz * 0.2 * vec3(1.0, -1.0, 1.0);
    gl_Position = vec4(pos, 1.0);
    outTexCoord = inTexCoord;
    outDepth = pos.z;
}
