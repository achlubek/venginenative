#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};
layout(set = 0, binding = 0) uniform UniformBufferObject {
    float Time;
    float Zero;
    vec2 Mouse;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexCoord;

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
	uint instance = gl_InstanceIndex;
		mat3 rotmat = rotationMatrix(vec3(0,1,0), ubo.Time * 0.1);
	rotmat = rotmat * rotationMatrix(vec3(1,0,0), ubo.Mouse.x * 4.0);
	rotmat = rotmat * rotationMatrix(vec3(0,0,1), ubo.Mouse.y * 4.0);
    gl_Position = vec4(rotmat * (inPosition * 0.05) + vec3(0.0, 0.0, 0.5), 1.0 + ubo.Zero);
    outNormal = rotmat * inNormal;
	outTexCoord = inTexCoord;
}