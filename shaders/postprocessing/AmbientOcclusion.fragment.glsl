#version 430 core

#include PostProcessEffectBase.glsl

vec2 xsamples[] = vec2[](
vec2(0.009011431, 0.09457164),
vec2(0.03588319, 0.1865808),
vec2(0.08012987, 0.2735036),
vec2(0.1409498, 0.3528925),
vec2(0.2172358, 0.422414),
vec2(0.3075902, 0.4798836),
vec2(0.4103443, 0.5232996),
vec2(0.5235803, 0.5508754),
vec2(0.6451581, 0.5610668),
vec2(0.7727448, 0.5525989),
vec2(0.03761707, 0.08723506),
vec2(0.09144463, 0.1665469),
vec2(0.1602457, 0.2356826),
vec2(0.2425058, 0.2925593),
vec2(0.3364545, 0.3352959),
vec2(0.4400912, 0.3622426),
vec2(0.5512127, 0.3720074),
vec2(0.6674456, 0.3634782),
vec2(0.7862788, 0.3358431),
vec2(0.9051006, 0.2886051),
vec2(0.06258768, 0.07146875),
vec2(0.1381696, 0.1304192),
vec2(0.2248766, 0.1750872),
vec2(0.3206278, 0.2039553),
vec2(0.4231609, 0.2157773),
vec2(0.5300651, 0.2095973),
vec2(0.6388161, 0.1847671),
vec2(0.7468139, 0.1409573),
vec2(0.8514194, 0.07816622),
vec2(0.9499944, -0.00327723),
vec2(0.08151028, 0.04879625),
vec2(0.1715428, 0.08168877),
vec2(0.2677771, 0.09757254),
vec2(0.3677669, 0.09564264),
vec2(0.4689762, 0.07540761),
vec2(0.5688174, 0.03669818),
vec2(0.6646892, -0.0203276),
vec2(0.7540158, -0.09518456),
vec2(0.8342854, -0.1870641),
vec2(0.9030879, -0.2948429),
vec2(0.09255636, 0.02140844),
vec2(0.1883395, 0.02506454),
vec2(0.2848017, 0.01062928),
vec2(0.3793677, -0.02191219),
vec2(0.4694732, -0.07224894),
vec2(0.5526036, -0.1397472),
vec2(0.6263317, -0.2234581),
vec2(0.6883554, -0.3221287),
vec2(0.7365322, -0.4342181),
vec2(0.7689138, -0.5579172),
vec2(0.09465848, -0.008048125),
vec2(0.1869365, -0.03398174),
vec2(0.2743052, -0.07734115),
vec2(0.3543093, -0.1373496),
vec2(0.4246039, -0.2129238),
vec2(0.4829903, -0.3026886),
vec2(0.5274503, -0.4049953),
vec2(0.5561774, -0.5179446),
vec2(0.5676062, -0.6394124),
vec2(0.5604377, -0.7670786),
vec2(0.08761352, -0.03672697),
vec2(0.1674693, -0.08974428),
vec2(0.2373019, -0.1578379),
vec2(0.2950131, -0.2395146),
vec2(0.338704, -0.3330234),
vec2(0.3667045, -0.4363804),
vec2(0.3776, -0.5473967),
vec2(0.3702547, -0.6637104),
vec2(0.3438309, -0.7828189),
vec2(0.297805, -0.9021155),
vec2(0.07210226, -0.06185681),
vec2(0.1318191, -0.1368346),
vec2(0.1773675, -0.2230824),
vec2(0.207209, -0.3185348),
vec2(0.2200743, -0.4209421),
vec2(0.2149831, -0.5279036),
vec2(0.1912613, -0.6369019),
vec2(0.1485533, -0.7453401),
vec2(0.08683044, -0.8505795),
vec2(0.006394804, -0.9499785),
vec2(0.04962357, -0.08100927),
vec2(0.08343099, -0.1707023),
vec2(0.1002937, -0.2667699),
vec2(0.09938195, -0.3667741),
vec2(0.08017833, -0.4681842),
vec2(0.04248739, -0.5684143),
vec2(-0.01355944, -0.6648617),
vec2(-0.08750311, -0.7549458),
vec2(-0.1785607, -0.8361466),
vec2(-0.2856334, -0.9060429),
vec2(0.02234963, -0.0923336),
vec2(0.02698069, -0.1880746),
vec2(0.01352821, -0.2846787),
vec2(-0.01804882, -0.3795711),
vec2(-0.06746549, -0.4701844),
vec2(-0.134114, -0.5539976),
vec2(-0.2170699, -0.6285743),
vec2(-0.3151039, -0.6915992),
vec2(-0.426697, -0.7409148),
vec2(-0.5500601, -0.7745541),
vec2(-0.007084003, -0.0947355),
vec2(-0.03207681, -0.1872727),
vec2(-0.07454451, -0.2750784),
vec2(-0.1337353, -0.3556893),
vec2(-0.20859, -0.4267496),
vec2(-0.2977556, -0.4860469),
vec2(-0.3996044, -0.5315461),
vec2(-0.5122554, -0.5614217),
vec2(-0.6336006, -0.5740865),
vec2(-0.7613332, -0.5682181),
vec2(-0.03583309, -0.08798289),
vec2(-0.08803466, -0.1683743),
vec2(-0.1554138, -0.2388965),
vec2(-0.2364988, -0.2974362),
vec2(-0.3295579, -0.3420769),
vec2(-0.4326245, -0.3711281),
vec2(-0.5435241, -0.3831534),
vec2(-0.6599066, -0.3769925),
vec2(-0.7792777, -0.3517829),
vec2(-0.8990367, -0.3069741),
vec2(-0.06111954, -0.07272827),
vec2(-0.1354855, -0.1332054),
vec2(-0.2212651, -0.1796295),
vec2(-0.3164087, -0.2104413),
vec2(-0.4186798, -0.2243484),
vec2(-0.5256876, -0.2203465),
vec2(-0.6349217, -0.1977355),
vec2(-0.7437891, -0.1561338),
vec2(-0.8496514, -0.09548577),
vec2(-0.9498642, -0.01606606),
vec2(-0.08049984, -0.05044575),
vec2(-0.169844, -0.08516456),
vec2(-0.265735, -0.1030045),
vec2(-0.3657433, -0.1031108),
vec2(-0.4673436, -0.08494069),
vec2(-0.5679523, -0.04827199),
vec2(-0.6649653, 0.006789881),
vec2(-0.7557976, 0.0798124),
vec2(-0.8379211, 0.1700387),
vec2(-0.9089038, 0.2763945),
vec2(-0.09210127, -0.02328855),
vec2(-0.1877901, -0.0288941),
vec2(-0.2845263, -0.01642592),
vec2(-0.3797352, 0.01418343),
vec2(-0.4708469, 0.06267501),
vec2(-0.5553344, 0.1284669),
vec2(-0.6307517, 0.2106591),
vec2(-0.6947714, 0.3080465),
vec2(-0.7452206, 0.4191316),
vec2(-0.7801142, 0.5421457),
vec2(-0.09480272, 0.006119081),
vec2(-0.1875896, 0.03016846),
vec2(-0.2758231, 0.07173992),
vec2(-0.3570324, 0.1301071),
vec2(-0.4288512, 0.2042343),
vec2(-0.4890532, 0.2927917),
vec2(-0.5355871, 0.3941718),
vec2(-0.5666082, 0.5065128),
vec2(-0.5805076, 0.6277228),
vec2(-0.5759399, 0.7555087),
vec2(-0.08834317, 0.03493541),
vec2(-0.1692619, 0.08631578),
vec2(-0.2404665, 0.1529734),
vec2(-0.2998287, 0.2334582),
vec2(-0.3454146, 0.3260579),
vec2(-0.3755136, 0.4288234),
vec2(-0.3886675, 0.5395948),
vec2(-0.3836918, 0.656034),
vec2(-0.3596989, 0.7756557),
vec2(-0.3161118, 0.8958646),
vec2(-0.07334682, 0.06037585),
vec2(-0.134578, 0.1341222),
vec2(-0.1818731, 0.2194246),
vec2(-0.2136519, 0.3142497),
vec2(-0.2285998, 0.4163738),
vec2(-0.2256874, 0.5234168),
vec2(-0.20419, 0.6328755),
vec2(-0.163699, 0.7421607),
vec2(-0.1041316, 0.8486352),
vec2(-0.02573633, 0.9496514),
vec2(-0.05126279, 0.07998203),
vec2(-0.08688951, 0.1689681),
vec2(-0.1057049, 0.2646724),
vec2(-0.1068294, 0.3646744),
vec2(-0.0896948, 0.4664545),
vec2(-0.0540524, 0.5674313),
vec2(1.882498E-05, 0.665),
vec2(0.07211307, 0.756571),
vec2(0.1614982, 0.839609),
vec2(0.2671252, 0.9116711),
vec2(-0.02422512, 0.09185936),
vec2(-0.03080469, 0.1874862),
vec2(-0.01932213, 0.2843443),
vec2(0.01031622, 0.3798599),
vec2(0.05787758, 0.4714607),
vec2(0.1228058, 0.5566136),
vec2(0.2042258, 0.6328639),
vec2(0.300956, 0.6978721),
vec2(0.411522, 0.7494496),
vec2(0.5341747, 0.7855937),
vec2(0.005153478, 0.09486011),
vec2(0.02825686, 0.1878871),
vec2(0.06892776, 0.2765393),
vec2(0.126465, 0.3583387),
vec2(0.1998571, 0.4309085),
vec2(0.2877969, 0.492009),
vec2(0.388698, 0.5395728),
vec2(0.5007175, 0.5717359),
vec2(0.6217795, 0.5868691),
vec2(0.749605, 0.583603)
);

vec2 aoSamplesSpeed[] = vec2[](
vec2(0.5, 0.5),

vec2(0.25, 0.5),
vec2(0.75, 0.5),

vec2(0.5, 0.25),
vec2(0.5, 0.75),

vec2(0.25, 0.25),
vec2(0.75, 0.75),

vec2(0.75, 0.25),
vec2(0.75, 0.25),


vec2(0.33, 0.5),
vec2(0.66, 0.5),

vec2(0.5, 0.33),
vec2(0.5, 0.66),

vec2(0.33, 0.33),
vec2(0.66, 0.66),

vec2(0.66, 0.33),
vec2(0.66, 0.33)
);
vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

float rand2s(vec2 co){
        return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

float fastAO(float hemisphereSize, int quality){
    float ratio = Resolution.y/Resolution.x;
    float outc = 0.0;
    
    float xaon = currentData.cameraDistance;
    float factor = 1.0 / (xaon +1.0);
    vec2 multiplier = vec2(ratio, 1) * 0.03 * factor * hemisphereSize;
    
    float rot = rand2s(UV) * 3.1415 * 2;
	
	vec3 normalcenter = currentData.normal;
    vec2 normproj = normalize(projectvdao(currentData.worldPos + normalcenter * 0.05) - UV);
	vec3 projref = normalize(reflect(currentData.cameraPos, currentData.normal));
    vec2 refproj = normalize(projectvdao(currentData.worldPos + projref * 0.05) - UV);
    float iter = 0.0;
    float stepsz = 1.0 / xsamples.length();
		
    mat2 RM = mat2(cos(rot), -sin(rot), sin(rot), cos(rot));
    for(int g=0;g < xsamples.length();g+=quality){
        vec2 sampl = RM * xsamples[g];
        sampl = faceforward(sampl, -sampl, normproj) * multiplier;
       // sampl = mix(refproj  * iter * 0.4, sampl * multiplier, currentData.roughness);
		vec2 nuv = UV + sampl;
		//if(nuv.x > 1.0 || nuv.x < 0.0 || nuv.y > 1.0 || nuv.y<0.0) continue;
        float aondata = texture(mrt_Distance_Bump_Tex, nuv).r;    
        vec3 normdata = texture(mrt_Normal_Metalness_Tex, nuv).rgb;        
		
		vec3 dir = normalize((FrustumConeLeftBottom + FrustumConeBottomLeftToBottomRight * nuv.x + FrustumConeBottomLeftToTopLeft * nuv.y));
		vec3 newpos = dir * aondata;
        
		float indirectAmount =  max(0, dot(normalcenter, normdata)) ;
      
		float occ = max(0, dot(normalize(newpos - currentData.cameraPos), normalcenter)) * indirectAmount;
      //  float occ =  smoothstep(0.0, hemisphereSize, xaon - aondata);
		
		float fact =  1.0 - clamp(distance(newpos, currentData.cameraPos) - 10.1, 0.0, 1.0);
		outc += occ * fact;
        iter += stepsz;
    
    }
    return outc / (float(xsamples.length()) / (float(quality)));
}
uniform float Time;
uniform vec3 SunDirection;
float rdhash = 0.453451 + Time;
vec2 randpoint2(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    return vec2(x, y) * 2.0 - 1.0;
}
vec3 randpoint3(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    float z = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    return vec3(x * 2.0 - 1.0, y, z * 2.0 - 1.0);
}
float slowao(float AOSCALE){
    float ao = 0.0;
    float d = 0.0;
    float w = 0.001;
    float nw = 1.0;
    vec3 refdir = normalize(reflect(currentData.cameraPos, currentData.normal));
    vec3 refrefdir = normalize(mix(refdir, currentData.normal, currentData.roughness));
    float sca = mix(12.0, AOSCALE, currentData.roughness);
    for(int i=0;i<32;i++){
        vec3 p = randpoint3();
       // p *= sign(dot(refdir, p));
        //p *= sign(dot(vec3(0.0, 1.0,0.0), p));
       // p *= sign(dot(refdir, p));
       // p *= sign(dot(vec3(0.0, 1.0, 0.0), p));
        p = mix(refdir, p, currentData.roughness);
        p *= sca;
        vec2 nuv = projectvdao(currentData.worldPos + p);
        nuv = clamp(nuv, 0.01, 0.99);
        p = FromCameraSpace(reconstructCameraSpaceDistance(nuv, textureLod(mrt_Distance_Bump_Tex, nuv, 1).r));
        d = pow(max(0.0, dot(refrefdir, normalize(p - currentData.worldPos))), 1.0 + 12.0 * (1.0 - currentData.roughness));
        nw = 1.0 - smoothstep(0.0, sca * 0.5, distance(currentData.worldPos, p));
        ao = max(d * nw, ao);
        //w += nw;
        //ao += d * nw;//nw * smoothstep(0.0, 1.0, d);
    }
    ao = 1.0 - ao;
    return mix(ao, pow(ao, 4.0), currentData.roughness);
}

float AmbientOcclusion(){
    //float ao = AO(currentData.worldPos, currentData.cameraPos, currentData.normal, currentData.roughness, 8.4,3));
    float ao = slowao(300.0);
    ao += slowao(200.0);
    ao += slowao(100.0);
    return ao * 0.333;
    ao = 1.0 - pow(1.0 - ao, 2.0);
   // ao += AO(currentData.worldPos, currentData.cameraPos, currentData.normal, currentData.roughness, 2.4,4);
    //ao *= 0.5;
    #define aolog 1.0
    return clamp(1.0 - ( log2(ao*aolog + 1.0) / log2(aolog + 1.0) ), 0.0, 1.0);
}

vec4 shade(){
    vec4 color = vec4(1);
    if(currentData.cameraDistance > 0){
        color.r = clamp(AmbientOcclusion(), 0.0, 1.0);
    }
    return color;
}