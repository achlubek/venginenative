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
} cameraBuffer;

vec3 getSky(vec3 position, vec3 direction){
    return vec3(0.5);
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
    quat_mul_vec(quatrotation, displacement);
    return point + displacement;
}

vec3 boxNormal(Box box, vec3 point){
    vec3 direction = point - box.position_zero.xyz;
    direction = normalize(quat_mul_vec(box.orientation, direction));

    vec3 px = quat_mul_vec(box.orientation, vec3(1.0, 0.0, 0.0));
    vec3 nx = quat_mul_vec(box.orientation, vec3(-1.0, 0.0, 0.0));
    vec3 py = quat_mul_vec(box.orientation, vec3(0.0, 1.0, 0.0));
    vec3 ny = quat_mul_vec(box.orientation, vec3(0.0, -1.0, 0.0));
    vec3 pz = quat_mul_vec(box.orientation, vec3(0.0, 0.0, 1.0));
    vec3 nz = quat_mul_vec(box.orientation, vec3(0.0, 0.0, -1.0));

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

    if(mind == dpx) return px;
    if(mind == dnx) return nx;
    if(mind == dpy) return py;
    if(mind == dny) return ny;
    if(mind == dpz) return pz;
    if(mind == dnz) return nz;
    return px;
}

float intersectBox(Ray ray, Box box) {
    vec3 cubeMin = box.position_zero.xyz - box.scale_zero.xyz * 0.5;
    vec3 cubeMax = box.position_zero.xyz + box.scale_zero.xyz * 0.5;
    vec3 ro = rotateAroundPoint(ray.o, box.position_zero.xyz, box.orientation);
    vec3 tMin = (cubeMin - ray.o) / ray.d;
    vec3 tMax = (cubeMax - ray.o) / ray.d;
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
vec3 shadeBox(Ray ray, Box box, float distance) {
    vec3 intersectionPoint = ray.o + ray.d * distance;
    vec3 normal = boxNormal(box, intersectionPoint);
    return normal;
}

vec3 getCameraRay(vec2 uv){
    float fov = cameraBuffer.cameraPosition_fov.a;
    vec2 signeduv = uv * 2.0 - 1.0;
    return quat_mul_vec(cameraBuffer.cameraOrientation, vec3(signeduv.x, signeduv.y, -fov));
}

vec3 trace(vec2 uv){
    Ray ray = Ray(cameraBuffer.cameraPosition_fov.xyz, getCameraRay(uv));
    int boxesCount = boxBuffer.boxes.length();
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
    return shadeBox(ray, boxBuffer.boxes[minDistBoxIndex], minDist);
}

void main() {
    outColor = vec4(trace(UV), 1.0);
}
