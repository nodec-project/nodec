

// ================================== BRDF NOTES =========================================
//	src: https://learnopengl.com/#!PBR/Theory
//	ref: http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
//
// Rendering Equation
// ------------------
//
//		L_o(p, w_o) = L_e(p) + Integral_Omega()[f_r(p, w_i, w_o) * L_i(p, w_i) * dot(N, w_i)]dw_i
//
// L_o		: Radiance leaving surface point P along the direction w_o (V=eye)
// L_e		: Emissive Radiance leaving surface point p | we're not gonna use emissive materials for now 
// L_i		: Irradiance (Radiance coming) coming from a light source at point p 
// f_r()	: reflectance equation representing the material property
// Integral : all incoming radiance over hemisphere omega, reflected towards the direction of w_o<->(V=eye) 
//			  by the surface material
//
// The integral is numerically solved.
// 
// BRDF						: Bi-directional Reflectance Distribution Function
// The Cook-Torrence BRDF	: f_r = k_d * f_lambert + k_s * f_cook-torrence
//							f_lambert = albedo / PI;
//

#define PI 3.14159265359f

// constants
#define EPSILON 0.000000000001f

struct BRDFSurface
{
    float3 normal;
    float3 albedo;
    float roughness;
    float metalness;
};

// Trowbridge-Reitz GGX Distribution
inline float NormalDistributionGGX(float3 normal, float3 lightHalfSolidAngle, float roughness)
{
    // approximates microfacets :	approximates the amount the surface's microfacets are
	//								aligned to the halfway vector influenced by the roughness
	//								of the surface
	//							:	determines the size, brightness, and shape of the specular highlight
	// more: http://reedbeta.com/blog/hows-the-ndf-really-defined/
	//
	// NDF_GGXTR(N, H, roughness) = roughness^2 / ( PI * ( dot(N, H))^2 * (roughness^2 - 1) + 1 )^2
    const float a = roughness * roughness;
    const float a2 = a * a;
    const float nh2 = pow(max(dot(normal, lightHalfSolidAngle), 0), 2);
    const float denom = (PI * pow((nh2 * (a2 - 1.0f) + 1.0f), 2));
    if (denom < EPSILON)
        return 1.0f;

    return a2 / denom;
}

// Smith's Schlick-GGX for Direct Lighting (non-IBL)
inline float GeometrySmithsSchlickGGX(float3 normal, float3 viewDir, float roughness)
{
    // describes self shadowing of geometry
	//
	// G_ShclickGGX(N, V, k) = ( dot(N,V) ) / ( dot(N,V)*(1-k) + k )
	//
	// k		 :	remapping of roughness based on wheter we're using geometry function 
	//				for direct lighting or IBL
	// k_direct	 = (roughness + 1)^2 / 8
	// k_IBL	 = roughness^2 / 2
	//
    const float k = pow((roughness + 1.0f), 2) / 8.0f;
    const float nv = max(0.0f, dot(normal, viewDir));
    const float denom = (nv * (1.0f - k) + k) + 0.0001f;
    if (denom < EPSILON)
        return 1.0f;
    return nv / denom;
}

inline float Geometry(float3 normal, float3 lightInSolidAngle, float3 lightOutSolidAngle, float roughness)
{
    // essentially a multiplier [0, 1] measuring microfacet shadowing
    return GeometrySmithsSchlickGGX(normal, lightInSolidAngle, roughness) * GeometrySmithsSchlickGGX(normal, lightOutSolidAngle, roughness);
}


// Fresnel-Schlick approximation describes reflection
inline float3 Fresnel(float3 normal, float3 viewDir, float3 f0)
{
    // F_Schlick(N, V, F0) = F0 - (1-F0)*(1 - dot(N,V))^5
    return f0 + (float3(1, 1, 1) - f0) * pow(1.0f - max(0.0f, dot(normal, viewDir)), 5.0f);
}



// Fresnel-Schlick with roughness factored in used in image-based lighting 
// for factoring in irradiance coming from environment map
// src: https://seblagarde.wordpress.com/2011/08/17/hello-world/ 
float3 FresnelWithRoughness(float cosTheta, float3 f0, float roughness)
{
    return f0 + (max((1.0f - roughness).xxx, f0) - f0) * pow(1.0 - cosTheta, 5.0);
}

inline float3 FLambertDiffuse(float3 kd)
{
    return kd / PI;
}

// <https://qiita.com/mebiusbox2/items/1cd65993ffb546822213#brdf-%E3%81%AE%E6%80%A7%E8%B3%AA>
float SchlickFresnel(float u, float f0, float f90)
{
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float DisneyDiffuse(float albedo, float3 N, float3 L, float3 V, float roughness)
{
    float3 H = normalize(L + V);
    float dotLH = saturate(dot(L, H));
    float dotNL = saturate(dot(N, L));
    float dotNV = saturate(dot(N, V));
    float Fd90 = 0.5 + 2.0 * dotLH * dotLH * roughness;
    float FL = SchlickFresnel(1.0, Fd90, dotNL);
    float FV = SchlickFresnel(1.0, Fd90, dotNV);
    return (albedo * FL * FV) / PI;
}

// <https://light11.hatenadiary.com/entry/2020/03/05/220957>
inline float V_SmithGGXCorrelated(float ndotl, float ndotv, float alpha)
{
    float lambdaV = ndotl * (ndotv * (1 - alpha) + alpha);
    float lambdaL = ndotv * (ndotl * (1 - alpha) + alpha);
    return 0.5f / (lambdaV + lambdaL + 0.0001);
}
inline half3 F_Schlick(half3 f0, half cos)
{
    return f0 + (1 - f0) * pow(1 - cos, 5);
}

inline half Fd_Burley(half ndotv, half ndotl, half ldoth, half roughness)
{
    half fd90 = 0.5 + 2 * ldoth * ldoth * roughness;
    half lightScatter = (1 + (fd90 - 1) * pow(1 - ndotl, 5));
    half viewScatter = (1 + (fd90 - 1) * pow(1 - ndotv, 5));

    half diffuse = lightScatter * viewScatter / PI;
    return diffuse;
}

inline half D_GGX(half perceptualRoughness, half ndoth, half3 normal, half3 halfDir)
{
    half3 ncrossh = cross(normal, halfDir);
    half a = ndoth * perceptualRoughness;
    half k = perceptualRoughness / (dot(ncrossh, ncrossh) + a * a);
    half d = k * k / PI;
    return min(d, 65504.0);
}

float3 BRDF_2(BRDFSurface surface, float3 lightDir, float3 viewDir, float3 lightColor, float3 irradience, float3 envSpecular)
{
    const float3 normal = surface.normal;
    const float3 albedo = surface.albedo;
    const float metalness = surface.metalness;
    const float roughness = surface.roughness;
    
    float3 halfDir = normalize(lightDir + viewDir);
    half ndotv = abs(dot(normal, viewDir));
    float ndotl = max(0, dot(normal, lightDir));
    float ndoth = max(0, dot(normal, halfDir));
    half ldoth = max(0, dot(lightDir, halfDir));
    half reflectivity = lerp(0.04f.xxx, 1, metalness);
    half3 f0 = lerp(0.04f.xxx, albedo, metalness);
    
    // Diffuse
    half diffuseTerm = Fd_Burley(ndotv, ndotl, ldoth, roughness) * ndotl;
    half3 diffuse = albedo * (1 - reflectivity) * diffuseTerm * lightColor;
    
    // Indirect Diffuse
    diffuse += albedo * (1 - reflectivity) * irradience;
    
    // Specular
    float alpha = roughness * roughness;
    float V = V_SmithGGXCorrelated(ndotl, ndotv, alpha);
    float D = D_GGX(roughness, ndotv, normal, halfDir);
    float3 F = F_Schlick(f0, ldoth);
    float3 specular = V * D * F * ndotl * lightColor;

    specular = max(0, specular);
    
    // Indirect Specular
    half surfaceReduction = 1.0 / (alpha * alpha + 1.0);
    half f90 = saturate((1 - roughness) + reflectivity);
    specular += surfaceReduction * envSpecular * lerp(f0, f90, pow(1 - ndotv, 5));
                

    half3 color = diffuse + specular;
    return half4(color, 1);
}

float3 BRDF(BRDFSurface surface, float3 lightInSolidAngle, float3 viewDir)
{
    const float3 normal = surface.normal;
    const float3 lightOutSolidAngle = viewDir;
    const float3 lightHalfSolidAngle = normalize(lightInSolidAngle + lightOutSolidAngle);
    
    //// surface
    const float3 albedo = surface.albedo;
    const float roughness = surface.roughness;
    const float metalness = surface.metalness;
    const float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metalness);
    
    //// Fresnel_Cook-Torrence BRDF
    const float3 fresnel = Fresnel(lightHalfSolidAngle, viewDir, f0);
    const float geometory = Geometry(normal, lightInSolidAngle, lightOutSolidAngle, roughness);
    const float ndf = NormalDistributionGGX(normal, lightHalfSolidAngle, roughness);
    //const float ndf = 1.0f;
    const float denom = (4.0f * max(0.0f, dot(lightOutSolidAngle, normal)) * max(0.0f, dot(lightInSolidAngle, normal))) + 0.0001f;
    const float3 specular = ndf * fresnel * geometory / denom;
    //const float3 specular = fresnel;
    //////const float3 specular=1.0f.xxx;
    //////const float3 specular = float3(1, 1, 1);
    const float3 iSpecular = specular;
    //const float3 iSpecular = surface.specularColor;
    
    // Diffuse BRDF
    const float3 ks = fresnel;
    const float3 kd = (float3(1.0f, 1.0f, 1.0f) - ks) * (1.0f - metalness) * albedo;
    const float3 iDiffuse = FLambertDiffuse(kd);
    
    return (iDiffuse + iSpecular);
    //return iDiffuse + iSpecular;
    //return iDiffuse;
    //return float3(1, 1, 1);
    //return iSpecular;
}

float3 EnvironmentBRDF(BRDFSurface surface, float3 viewDir, float3 irradience, float3 envSpecular)
{
    const float3 f0 = lerp(0.04f.xxx, surface.albedo, surface.metalness);
    const float3 ks = FresnelWithRoughness(max(dot(surface.normal, viewDir), 0.0), f0, surface.roughness);
    const float kd = (1.0f - ks) * (1.0f - surface.metalness);
    
    const float3 diffuse = irradience * surface.albedo;
    const float3 specular = envSpecular * ks;

    return (kd * diffuse + specular);
}