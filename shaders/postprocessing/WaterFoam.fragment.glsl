#version 430 core

#include PostProcessEffectBase.glsl

#include Atmosphere.glsl
#include WaterHeight.glsl

vec3 normalx(vec3 pos, float e, float roughness){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwaterXOLO(pos.xz, vec2(0.0), 0.0) * waterdepth, pos.z);
    vec3 b = vec3(pos.x - e, heightwaterXOLO(pos.xz - ex.xy, vec2(0.0), 0.0) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwaterXOLO(pos.xz + ex.yx, vec2(0.0), 0.0) * waterdepth, pos.z + e);
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    return normalize(normal);
}

vec2 getFlowVelocity(vec3 pos){
    vec3 norm = normalx(pos, 0.01, 0.0);
    vec2 velocity = norm.xz * (1.0 - norm.y) * 1.0;
    return velocity;
}

float fbmHI2(vec3 p){
    p *= 0.1;
	float a = 0.0;
    float w = 1.0;
    float wc = 0.0;
	for(int i=0;i<5;i++){
		a += supernoise3d(p) * w;
		wc += w;
        w *= 0.6;
		p = p * 3.0;
	}
	return a / wc;
}
float getFoamGen(vec2 velocity, float n_dot_up, float y_delta){
    return (1.0 - smoothstep(0.0, 0.2, y_delta)) * length(velocity) * 100.0;
}

vec4 shade(){
    vec2 xz = AboveSpan * (UV * 2.0 - 1.0);
    vec2 velocity = getFlowVelocity(vec3(xz.x, 0.0, xz.y));
    vec2 uv = UV - normalize(velocity) * clamp(length(velocity) * 10.0, 0.0, 0.1) * 0.01 * WaterSpeed;
    //uv += (vec2(fbmHI2(vec3(xz.x, Time * 0.1, xz.y)), fbmHI2(vec3(-xz.x, Time * 0.1, -xz.y))) * 2.0 - 1.0) * 0.001;
    float above_n_dot_up = textureLod(aboveViewDataTex, uv, 0.0).b;
    float above_y = textureLod(aboveViewDataTex, uv, 0.0).r;
    vec3 pos = CameraPosition + vec3(-xz.x, above_y, xz.y);
    vec2 flow = heightflow(pos.xz) *1.1;
    uv += flow * 0.1;
    vec2 fakeNormalVec = getFlowVelocity(pos.xyz);

    float water_y = WaterLevel + heightwaterXOLO(pos.xz, vec2(0.0), 0.0).x * waterdepth;
    float y_delta = abs(water_y - above_y);

    float foamLast = textureLod(lastFoamTex, clamp(uv, 0.0, 1.0), 0.0).r * 1.0;
    foamLast += getFoamGen(flow * 11.0, above_n_dot_up, y_delta) * 10.0;
    foamLast += smoothstep(0.02, 0.06, length(flow)) * 1.0;

    return vec4(clamp(foamLast * 0.97, 0.0, 1.0), 0.0, 0.0, 0.0);
}
