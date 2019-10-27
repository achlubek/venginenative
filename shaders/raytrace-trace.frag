#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;


struct Box{
    vec4 position_zero;
    vec4 orientation;
    vec4 scale_zero;
    vec4 color_roughness;
};

layout(set = 0, binding = 0) buffer ssbo1 {
    Box boxes[];
} boxBuffer;

layout(set = 0, binding = 1) uniform ubo1 {
    vec4 cameraPosition_fov;
    vec4 cameraOrientation;
    ivec4 boxesCount;
    vec4 time;
} cameraBuffer;

float sun(vec3 ray, float roughness){
 	vec3 sd = normalize(vec3(-0.5, 0.5, 1.0));   
    return pow(max(0.0, dot(ray, sd)), 528.0 * (1.0 - roughness * roughness) + 1.0) * 110.0 * (1.0 - roughness * roughness * 0.998) * smoothstep(-0.07, 0.0, ray.y);
}

vec3 extra_cheap_atmosphere(vec3 raydir, vec3 sundir){
	sundir.y = max(sundir.y, -0.07);
	raydir.y = max(raydir.y, -0.07);
	float special_trick = 1.0 / (raydir.y * 1.0 + 0.1);
	float special_trick2 = 1.0 / (sundir.y * 11.0 + 1.0);
	float raysundt = pow(abs(dot(sundir, raydir)), 2.0);
	float sundt = pow(max(0.0, dot(sundir, raydir)), 8.0);
	float mymie = sundt * special_trick * 0.2;
	vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - vec3(5.5, 13.0, 22.4) / 22.4), special_trick2);
	vec3 bluesky= vec3(5.5, 13.0, 22.4) / 22.4 * suncolor;
	vec3 bluesky2 = max(vec3(0.0), bluesky - vec3(5.5, 13.0, 22.4) * 0.002 * (special_trick + -6.0 * sundir.y * sundir.y));
	bluesky2 *= special_trick * (0.24 + raysundt * 0.24);
	return bluesky2 * (1.0 + 1.0 * pow(1.0 - raydir.y, 3.0)) + mymie * suncolor * smoothstep(-0.07, 0.0, raydir.y);
} 
float fresnelCoefficent(vec3 surfaceDir, vec3 incomingDir, float baseReflectivity){
    return (baseReflectivity + (1.0 - baseReflectivity) * (pow(1.0 - max(0.0, dot(surfaceDir, -incomingDir)), 5.0)));
}

vec3 getSky(vec3 origin, vec3 ray, float roughness){
    //return extra_cheap_atmosphere(ray, normalize(vec3(-0.5, 0.5, 1.0))) + sun(ray, roughness);
    return sun(ray, roughness) * vec3(1.0);
}
struct Ray{
    vec3 o;
    vec3 d;
};

vec3 quat_mul_vec( vec4 q, vec3 v ){ 
	return v + 2.0*cross(cross(v, q.xyz ) + q.w*v, q.xyz);
}

vec3 rotateAroundPoint(vec3 target, vec3 point, vec4 quatrotation){
    vec3 displacement = target - point;
    return point + quat_mul_vec(quatrotation, displacement);
}

vec4 quat_inverse(vec4 quaternion) {
	float len = 1.0 / (quaternion.x * quaternion.x +
	                 quaternion.y * quaternion.y +
	                 quaternion.z * quaternion.z +
	                 quaternion.w * quaternion.w);
	quaternion.x *= -len;
	quaternion.y *= -len;
	quaternion.z *= -len;
	quaternion.w *= len;
    return quaternion;
}

vec3 boxNormal(Box box, vec3 point){
    vec3 direction = box.position_zero.xyz - point;
    direction = normalize(quat_mul_vec(box.orientation, direction) / box.scale_zero.xyz);

    vec3 px = vec3(1.0, 0.0, 0.0);
    vec3 nx = vec3(-1.0, 0.0, 0.0);
    vec3 py = vec3(0.0, 1.0, 0.0);
    vec3 ny = vec3(0.0, -1.0, 0.0);
    vec3 pz = vec3(0.0, 0.0, 1.0);
    vec3 nz = vec3(0.0, 0.0, -1.0);

    float dpx = distance(direction, px);
    float dnx = distance(direction, nx);
    float dpy = distance(direction, py);
    float dny = distance(direction, ny);
    float dpz = distance(direction, pz);
    float dnz = distance(direction, nz);

    float mind = 
    min(dpx,
        min(dpy,
            min(dpz,
                min(dnx,
                    min(
                        dny, 
                        dnz
                    )))));

    vec3 res = px;
    if(mind == dpx) res = px;
    if(mind == dnx) res = nx;
    if(mind == dpy) res = py;
    if(mind == dny) res = ny;
    if(mind == dpz) res = pz;
    if(mind == dnz) res = nz;
    return quat_mul_vec(quat_inverse(box.orientation), res);
}

float intersectBox(Ray ray, Box box) {
    vec3 cubeMin = box.position_zero.xyz - box.scale_zero.xyz * 0.5;
    vec3 cubeMax = box.position_zero.xyz + box.scale_zero.xyz * 0.5;
    vec3 ro = rotateAroundPoint(ray.o, box.position_zero.xyz, box.orientation);
    vec3 rd = quat_mul_vec(box.orientation, ray.d);
    vec3 tMin = (cubeMin - ro) / rd;
    vec3 tMax = (cubeMax - ro) / rd;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    float positiveMin = 99999.0;
    if(tNear > tFar) return positiveMin;
    if(tNear > 0.0 && tFar > 0.0) positiveMin = min(tNear, tFar);
    else if(tNear > 0.0 && tFar <= 0.0) positiveMin = tNear;
    else if(tNear <= 0.0 && tFar > 0.0) positiveMin = tFar;
    return positiveMin;
}

float rand2s(vec2 co){
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

float time = cameraBuffer.time.x;
float iter1 = 0.0 + time;
float iter2 = 1.1112+ time;
float iter3 = 0.4565+ time;
Ray generateRandomRay(Ray originalRay, vec3 normal, float roughness){
    vec3 rd = vec3(
        rand2s(UV + iter1),
        rand2s(UV + iter2),
        rand2s(UV + iter3)
    ) * 2.0 - 1.0;
    rd = faceforward(rd, -rd, normal);
    rd = mix(reflect(originalRay.d, normal), -rd, roughness);
    iter1 += 1.1234;
    iter2 += 1.24112;
    iter3 += 1.0565;
    return Ray(originalRay.o, normalize(rd));
}

vec3 shadeBoxColorRay(Ray ray, Box box, vec3 color, float distance) {
    vec3 intersectionPoint = ray.o + ray.d * distance;
    vec3 normal = boxNormal(box, intersectionPoint);
    float roughness = box.color_roughness.w;
    vec3 surfaceColor = box.color_roughness.rgb;
    return surfaceColor * color * fresnelCoefficent(normal, ray.d, 0.04);
}

vec3 getCameraRay(vec2 uv){
    float fov = cameraBuffer.cameraPosition_fov.a;
    vec2 signeduv = uv * 2.0 - 1.0;
    return quat_mul_vec(cameraBuffer.cameraOrientation, normalize(vec3(signeduv.x, -signeduv.y, -fov)));
}

struct Intersection
{
    Ray ray;
    Box box;
    vec3 hitPoint;
    vec3 normal;
};
const int MAX_INTERSECTIONS = 4;
Intersection intersections[MAX_INTERSECTIONS];
int tracingIndex = 0;

void traceIntersections(Ray ray){
    int boxesCount = cameraBuffer.boxesCount.w;
    int minDistBoxIndex = -1;
    do{
        minDistBoxIndex = -1;
        float minDist = 9999.0;
        for(int i=0;i<boxesCount;i++){
            Box box = boxBuffer.boxes[i];
            float dst = intersectBox(ray, box);
            if(dst < minDist){
                minDist = dst;
                minDistBoxIndex = i;
            }
        }
        if(minDistBoxIndex > -1){
            vec3 point = ray.o + ray.d * minDist;
            Box box = boxBuffer.boxes[minDistBoxIndex];
            vec3 normal = boxNormal(box, point);
            intersections[tracingIndex++] = Intersection(
                ray, 
                box,
                point,
                normal
            );
            float roughness = box.color_roughness.w;
            float fresnel = fresnelCoefficent(normal, ray.d, 0.04);
            Ray randomRay = generateRandomRay(ray, normal, roughness);
            randomRay.o = point + randomRay.d * 0.001;
            ray = randomRay;
        } else break;
    } while (minDistBoxIndex > -1 && tracingIndex < MAX_INTERSECTIONS);
}

vec3 resolveIntersections(Ray cameraRay){
    if(tracingIndex == 0){
        return getSky(cameraRay.o, cameraRay.d, 0.0);
    }
    Intersection initialIntersection = intersections[tracingIndex-1];
    Ray randomRay = generateRandomRay(initialIntersection.ray, initialIntersection.normal, initialIntersection.box.color_roughness.a);
    vec3 initialColor = initialIntersection.box.color_roughness.rgb * getSky(initialIntersection.hitPoint, randomRay.d, initialIntersection.box.color_roughness.a);
    for(int i = 0; i < tracingIndex - 1; i++){
        Intersection inter = intersections[i];
        initialColor *= fresnelCoefficent(inter.normal, inter.ray.d, 0.04) * inter.box.color_roughness.rgb;
    }
    return initialColor;
}

vec3 aces_tonemap(vec3 color){	
	mat3 m1 = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
	);
	mat3 m2 = mat3(
        1.60475, -0.10208, -0.00327,
        -0.53108,  1.10813, -0.07276,
        -0.07367, -0.00605,  1.07602
	);
	vec3 v = m1 * color;    
	vec3 a = v * (v + 0.0245786) - 0.000090537;
	vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
	return pow(clamp(m2 * (a / b), 0.0, 1.0), vec3(1.0 / 2.2));	
}

void main() {
    vec3 result = vec3(0.0);
    float width = 1000.0;
    float height = 1000.0;
    vec2 pixel = 1.0 / vec2(width, height);
    float w = 0.0;
    for(int x=0;x<6;x++){
        for(int y=0;y<6;y++){
            float fx = x / 6.0;
            float fy = y / 6.0;
            vec2 offset = pixel * vec2(fx, fy);
            Ray ray = Ray(cameraBuffer.cameraPosition_fov.xyz, getCameraRay(UV + offset));
            tracingIndex = 0;
            traceIntersections(ray);
            result += resolveIntersections(ray);
            //result += (float(tracingIndex) / 2.0);
            w += 1.0;
        }
    }
    outColor = vec4(aces_tonemap(result / w), 1.0);
}
