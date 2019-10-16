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

float sun(vec3 ray){
 	vec3 sd = normalize(vec3(1.0));   
    return pow(max(0.0, dot(ray, sd)), 528.0) * 110.0;
}

vec3 extra_cheap_atmosphere(vec3 raydir, vec3 sundir){
	sundir.y = max(sundir.y, -0.07);
	float special_trick = 1.0 / (raydir.y * 1.0 + 0.1);
	float special_trick2 = 1.0 / (sundir.y * 11.0 + 1.0);
	float raysundt = pow(abs(dot(sundir, raydir)), 2.0);
	float sundt = pow(max(0.0, dot(sundir, raydir)), 8.0);
	float mymie = sundt * special_trick * 0.2;
	vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - vec3(5.5, 13.0, 22.4) / 22.4), special_trick2);
	vec3 bluesky= vec3(5.5, 13.0, 22.4) / 22.4 * suncolor;
	vec3 bluesky2 = max(vec3(0.0), bluesky - vec3(5.5, 13.0, 22.4) * 0.002 * (special_trick + -6.0 * sundir.y * sundir.y));
	bluesky2 *= special_trick * (0.24 + raysundt * 0.24);
	return bluesky2 * (1.0 + 1.0 * pow(1.0 - raydir.y, 3.0)) + mymie * suncolor;
} 
float fresnelCoefficent(vec3 surfaceDir, vec3 incomingDir, float baseReflectivity){
    return (baseReflectivity + (1.0 - baseReflectivity) * (pow(1.0 - max(0.0, dot(surfaceDir, -incomingDir)), 5.0)));
}

vec3 getSky(vec3 origin, vec3 ray){
    float mult = 1.0;
    if(ray.y < 0.0){
        mult = fresnelCoefficent(ray, vec3(0.0, 1.0, 0.0), 0.04);
        ray = reflect(ray, vec3(0.0, 1.0, 0.0));
    }
 	return extra_cheap_atmosphere(ray, normalize(vec3(1.0))) * 0.5 + sun(ray);
    
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
    direction = normalize(quat_mul_vec(box.orientation, direction));

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
    rd = mix(reflect(originalRay.d, normal), rd, roughness);
    return Ray(originalRay.o, normalize(rd));
}

vec3 trace(Ray ray, int counter);

vec3 shadeBox(Ray ray, Box box, float distance, int counter) {
    vec3 intersectionPoint = ray.o + ray.d * distance;
    vec3 normal = boxNormal(box, intersectionPoint);
    float roughness = box.color_roughness.w;
    vec3 color = box.color_roughness.rgb;

    if(counter == 0){
        return getSky(ray.o, ray.d) * fresnelCoefficent(normal, ray.d, 0.04);
    } else {
        vec3 result = vec3(0.0);
        for(int i=0;i<4;i++){
            float fresnel = fresnelCoefficent(normal, ray.d, 0.04);
            Ray randomRay = generateRandomRay(ray, normal, roughness - fresnel * (1.0 - roughness));
            result += trace(randomRay, counter - 1) * fresnel;
        }
        return result / 4.0;
    }
}

vec3 getCameraRay(vec2 uv){
    float fov = cameraBuffer.cameraPosition_fov.a;
    vec2 signeduv = uv * 2.0 - 1.0;
    return quat_mul_vec(cameraBuffer.cameraOrientation, normalize(vec3(signeduv.x, -signeduv.y, -fov)));
}

vec3 trace(Ray ray, int counter){
    int boxesCount = cameraBuffer.boxesCount.w;
    int minDistBoxIndex = -1;
    float minDist = 9999.0;
    for(int i=0;i<boxesCount;i++){
        Box box = boxBuffer.boxes[i];
        float dst = intersectBox(ray, box);
        if(dst < minDist){
            minDist = dst;
            minDistBoxIndex = i;
        }
    }
    if(minDistBoxIndex == -1){
        return getSky(ray.o, ray.d);
    }
    return shadeBox(ray, boxBuffer.boxes[minDistBoxIndex], minDist, counter);
}

void main() {
    vec3 result = vec3(0.0);
    float width = 1000.0;
    float height = 1000.0;
    vec2 pixel = 1.0 / vec2(width, height);
    float w = 0.0;
    for(int x=0;x<4;x++){
        for(int y=0;y<4;y++){
            float fx = x / 4.0;
            float fy = x / 4.0;
            vec2 offset = pixel * vec2(fx, fy);
            Ray ray = Ray(cameraBuffer.cameraPosition_fov.xyz, getCameraRay(UV + offset));
            result += trace(ray, 3);
            w += 1.0;
        }
    }
    outColor = vec4(result / w, 1.0);
}
