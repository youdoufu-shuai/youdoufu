#version 300 es
precision highp float;
//precision mediump float;

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
//in vec3 inTangent;
//in vec3 inBitTangent;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;

// material parameters
uniform sampler2D texture_diffuse1;   // albedoMap;
uniform sampler2D texture_normal1;    // normalMap;
uniform sampler2D texture_ao1;        // aoMap;



uniform vec3 camPos;

uniform float f_alpha;      // alpha value

uniform float f_alpha_lampshade_factor;
uniform float f_alpha_lampshade_threshold;

uniform bool b_use_texture_normal1;
uniform bool b_use_texture_ao1;


// const float PI = 3.14159265359;
#define PI 3.1415926535897932384626433832795

//#define ENABLE_LUMIN_RECTIFICATION

#define TONEMAPPING_REINHARD     0
#define TONEMAPPING_EXPOSURE     1
#define TONEMAPPING_ACES         2
#define TONEMAPPING_ACES_STEPHEN 3
#define TONEMAPPING_UNCHARTED2   4
#define TONEMAPPING_JIM_RICHARD  5
#define TONEMAPPING_CE           6

#define HDR_TONEMAPPING      TONEMAPPING_REINHARD
// #define HDR_TONEMAPPING      TONEMAPPING_EXPOSURE
// #define HDR_TONEMAPPING      TONEMAPPING_ACES
// #define HDR_TONEMAPPING      TONEMAPPING_ACES_STEPHEN
// #define HDR_TONEMAPPING      TONEMAPPING_UNCHARTED2
// #define HDR_TONEMAPPING      TONEMAPPING_JIM_RICHARD
// #define HDR_TONEMAPPING      TONEMAPPING_CE

#define ACES_LUMIN           1.0 // default 1.0
#define EXPOSURE             1.0 // default 1.0



vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(texture_normal1, TexCoords).xyz * 2.0 - 1.0;

    tangentNormal.x *= -1.0; // todo
    // tangentNormal.y *= -1.0; // todo
    // tangentNormal.z *= -1.0; // todo

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = PI * denom * denom;

    return nom / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}


float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    color        *= adapted_lum;

    return clamp((color * (A * color + B)) / (color * (C * color + D) + E), 0.0, 1.0);
}

vec3 Uncharted2Kernel(vec3 x)
{
	const float A = 0.22;
	const float B = 0.30;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.01;
	const float F = 0.30;
 
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 Uncharted2ToneMapping(vec3 color, float adapted_lum)
{
	const vec3 WHITE = vec3(11.2);
	return Uncharted2Kernel(adapted_lum * color) / Uncharted2Kernel(WHITE);
}

vec3 CEToneMapping(vec3 color, float adapted_lum) 
{
    return vec3(1.0) - exp(-adapted_lum * color);
}


const mat3 ACESInputMat = mat3(
    0.59719, 0.07600, 0.02840,
    0.35458, 0.90834, 0.13383,
    0.04823, 0.01566, 0.83777
);

const mat3 ACESOutputMat = mat3(
    1.60475,-0.10208,-0.00327,
    -0.53108, 1.10813,-0.07276,
    -0.07367,-0.00605, 1.07602
);

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
    color = ACESInputMat * color;

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = ACESOutputMat * color;

    // Clamp to [0, 1]
    color = clamp(color, 0.0, 1.0);

    return color;
}

vec3 LinearTosRGB(vec3 color)
{
    vec3 x = color * 12.92;
    vec3 y = 1.055 * pow(clamp(color, 0.0, 1.0), vec3(1.0 / 2.4)) - 0.055;

    vec3 clr = color;
    clr.r = color.r < 0.0031308 ? x.r : y.r;
    clr.g = color.g < 0.0031308 ? x.g : y.g;
    clr.b = color.b < 0.0031308 ? x.b : y.b;

    return clr;
}


void main()
{		
    // material properties
    vec4 albedoVec4  = texture(texture_diffuse1, TexCoords);
    vec3 albedo      = pow(albedoVec4.rgb, vec3(2.2));
    float metallic1  = 0.0;
    float roughness1 = 1.0;
    float ao1        = 1.0;
    vec3 arm1        = vec3(0.0, 0.0, 0.0);

    float factor_pbr = 0.0;

    if (b_use_texture_ao1)
    {
        arm1       = texture(texture_ao1, TexCoords).rgb;
        ao1        = arm1.r;
        roughness1 = arm1.g;
        metallic1  = arm1.b;

        factor_pbr = 1.0;
    }


    vec3 N;
    if (b_use_texture_normal1)
    {
        N         = getNormalFromMap();
        // metallic1  = 1.0;
        // roughness1 = 1.0;
    }
    else
    {
        N         = normalize(Normal);
        // metallic1  = 0.0;
        // roughness1 = 1.0;
    }


    // N = getNormalFromMap();
    // N = normalize(Normal);

    vec3 V = normalize(camPos - WorldPos);

    // view normal correction
    float nv  = dot(N, V);
    N        *= sign(nv);

    vec3 R = reflect(-V, N);

    // metallic1 workflow
   
    vec3 F0  = vec3(0.04); 
    // if (b_use_texture_ao1)
    // {
    //     F0  = vec3(0.56, 0.57, 0.58);
    // }

    F0       = mix(F0, albedo, metallic1);

    vec3 Lo  = vec3(0.0);
    
    vec3 F   = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness1);
    vec3 kS  = F;
    vec3 kD  = 1.0 - kS;
    kD      *= 1.0 - metallic1;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 8.0;

    float lod = roughness1 * MAX_REFLECTION_LOD;// * 2.138;
   
    vec3 prefilteredColor = textureLod(prefilterMap, R,  lod).rgb;


    vec2 brdf             = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness1)).rg;
    vec3 specular         = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = vec3(0.0, 0.0, 0.0);

#if defined(ENABLE_LUMIN_RECTIFICATION)
    ambient = (kD * diffuse + specular * 0.99) * ao1;

    #define K_RECTIFICATION 0.90
    float lumin = dot(ambient, vec3(0.299, 0.587, 0.114));
    lumin = clamp(lumin, 0.0, 1.0);

    Lo += pow(texture(texture_diffuse1, TexCoords).rgb, vec3(2.2)) * roughness1 * K_RECTIFICATION * dot(N, V) * (1.0-lumin);
#else
    ambient = (kD * diffuse + specular) * ao1;
#endif


    vec3 color   = ambient + Lo;

    color = mix(albedo, color, factor_pbr);

    #if (HDR_TONEMAPPING == TONEMAPPING_REINHARD)
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    #endif

    #if (HDR_TONEMAPPING == TONEMAPPING_EXPOSURE)
    color = vec3(1.0) - exp(-color.rgb * EXPOSURE);
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    #endif

    #if (HDR_TONEMAPPING == TONEMAPPING_ACES)
    color = ACESToneMapping(color, ACES_LUMIN);
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    #endif

    #if (HDR_TONEMAPPING == TONEMAPPING_ACES_STEPHEN)
    color = ACESFitted(color * 1.8); // 1.8
    // gamma correct
    // color = pow(color, vec3(1.0/2.2));
    color = LinearTosRGB(color);
    #endif

    #if (HDR_TONEMAPPING == TONEMAPPING_UNCHARTED2)
    color = Uncharted2ToneMapping(color, 1.0);
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    #endif

    #if (HDR_TONEMAPPING == TONEMAPPING_JIM_RICHARD)
    float exposure = 1.0;
    vec3 finalColor = min(color.rgb, 50.0 / exposure);
	finalColor *= exposure;

	vec3 x = max(vec3(0.), finalColor - vec3(0.004));
	color = (x * (6.2 * x + .49)) / (x * (6.175 * x + 1.7) + 0.06);
    #endif

    #if (HDR_TONEMAPPING == TONEMAPPING_CE)
    color = CEToneMapping(color, 1.0);
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    // color = LinearTosRGB(color);
    #endif
   

    float lampshadeAlpha = f_alpha;
    if (nv > f_alpha_lampshade_threshold)
    {
        lampshadeAlpha *= 0.01;
    }

    float alpha = mix(f_alpha, lampshadeAlpha, f_alpha_lampshade_factor);
   

    // FragColor = vec4(color, albedoVec4.a * alpha);
    FragColor = vec4(color, albedoVec4.a * alpha);
    // FragColor = vec4(color, albedoVec4.a * alpha).bgra;
    // FragColor = texture(texture_diffuse1, TexCoords).rgba;
}
