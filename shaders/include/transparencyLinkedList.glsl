#pragma once
struct Fragment{
    float depth;
    float alpha;
    vec3 color;
};

Fragment fragments[3] = Fragment[3](
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0))
);
int fragmentIndex = 0;

void storeFragment(float depth, float alpha, vec3 color){
    if(fragmentIndex < 3){
        fragments[fragmentIndex++] = Fragment(depth, alpha, color);
    }
}

Fragment findMaximum(float limit){
    Fragment fragment = Fragment(0.0, 0, vec3(0.0));
    for(int i=0;i<fragmentIndex;i++){
        if(fragments[i].depth > fragment.depth && fragments[i].depth < limit){
            fragment = fragments[i];
        }
    }
    return fragment;
}

vec4 resolveFragments(){
    vec4 result = vec4(0.0);
    float lastmin = 99999999.0;
    for(int i=0;i<fragmentIndex;i++){
        Fragment f = findMaximum(lastmin);
        lastmin = f.depth - 0.0001;
        result.xyz = mix(result.xyz, f.color, f.alpha);
        result.a = max(result.a, f.alpha);
    }
    return result;
}
