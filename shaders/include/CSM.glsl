
layout(binding = 24) uniform sampler2DArray CSMTex;

#define MAX_CSM_LAYERS 12
uniform mat4 CSMVPMatrices[MAX_CSM_LAYERS];
uniform float CSMRadiuses[MAX_CSM_LAYERS];
uniform int CSMLayers;


vec3 CSMProject(vec3 pos, int layer){
    vec4 tmp = ((CSMVPMatrices[layer]) * vec4(pos, 1.0));
    //tmp.xy /= tmp.w;
    return (tmp.xyz);
}
float rdhasha = Time * 0.1;
vec3 randpointx(){
    float x = rand2s(UV * rdhasha);
    rdhasha += 2.1231255;
    float y = rand2s(UV * rdhasha);
    rdhasha += 1.6271255;
    return vec3(x, y, 0.5) * 2.0 - 1.0;
}
float giscale = 1.0; 
float blurshadowabit(float blurscale, int samples, vec3 uv, float d){
    float v = 0.0;
    const float w = 1.0 / float(samples);
    for(int i=0;i<samples;i++){
        v += smoothstep(0.0, 0.0032, d - texture(CSMTex, uv + randpointx() * 0.24115 * giscale * blurscale).r);
        //  v += 1.0 - max(0, sign(d - texture(CSMTex, uv + randpointx() * 0.00115).r));
    }
    return v * w;
}   
float blurshadowabitGI(vec3 uv, float d){
    float v = 0.0;
    const float w = 1.0 / 24.0;
    for(int i=0;i<24;i++){
        v += smoothstep(0.0, 0.0022, d - texture(CSMTex, uv + randpointx() * 11.6115 * giscale).r);
        //  v += 1.0 - max(0, sign(d - texture(CSMTex, uv + randpointx() * 0.00115).r));
    }
    return v * w;
}

float CSMQueryVisibility(vec3 pos){
    //return CSMVPMatrices[0][0].x;
    //return 1.0;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
        // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
            giscale = 1.0 / CSMRadiuses[i];
            return 1.0 - blurshadowabit(1.0, 2, vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMQueryVisibilityVariable(vec3 pos, int samples, float blurscale){
    //return CSMVPMatrices[0][0].x;
    //return 1.0;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
        // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
            giscale = 1.0 / CSMRadiuses[i];
            return 1.0 - blurshadowabit(blurscale, samples, vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMQueryVisibilityGI(vec3 pos){
    //return CSMVPMatrices[0][0].x;
    //return 1.0;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
        // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
            giscale = 1.0 / CSMRadiuses[i];
            return 1.0 - blurshadowabitGI(vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMQueryVisibilitySimple(vec3 pos){
//return CSMVPMatrices[0][0].x;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
       // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;  
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
          //  return 1.0 - blurshadowabit(vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
            return 1.0 - max(0, sign(d - texture(CSMTex, vec3(csmuv.x, csmuv.y, float(i))).r - 0.0012));// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMReconstructDistance(vec3 pos){
    //return CSMVPMatrices[0][0].x;
    for(int i=0;i<CSMLayers;i++){
        vec3 csmuv = CSMProject(pos, i);
        // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float z = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
            float d = texture(CSMTex, vec3(csmuv.x, csmuv.y, float(i))).r;
            vec4 cp = vec4(csmuv.x, csmuv.y, d, 1.0) * 2.0 - 1.0;
            vec4 wp = (inverse(CSMVPMatrices[i]) * cp);
            return distance(wp.xyz / wp.w, pos);
        }
    }
    return 1.0;
}