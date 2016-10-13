#ifndef CONSTANTS_H
#define CONSTANTS_H
#define LN2 0.69314718055994530941
#define LN10 2.30258509299404568401
#define EULER 2.7182818284590452353602874
#define SQRT2 1.41421356237309504880
#define HALFSQRT2 .70710678118654752440
#define PI 3.141592653589793238462643
#define QUARTPI 0.78539816339744830962
#define HALFPI 1.57079632679489661923
#define TWOPI 6.28318530717958647692
#define RADPDEG 0.01745329251994329576
vec3 VECTOR_UP = vec3(0.0, 1.0, 0.0);
vec3 VECTOR_DOWN = vec3(0.0, -1.0, 0.0);
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
#endif
