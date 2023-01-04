

// ================================== BRDF NOTES =========================================
//    src: https://learnopengl.com/#!PBR/Theory
//    ref: http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
//
// Rendering Equation
// ------------------
//
//        L_o(p, w_o) = L_e(p) + Integral_Omega()[f_r(p, w_i, w_o) * L_i(p, w_i) * dot(N, w_i)]dw_i
//
// L_o        : Radiance leaving surface point P along the direction w_o (V=eye)
// L_e        : Emissive Radiance leaving surface point p | we're not gonna use emissive materials for now 
// L_i        : Irradiance (Radiance coming) coming from a light source at point p 
// f_r()    : reflectance equation representing the material property
// Integral : all incoming radiance over hemisphere omega, reflected towards the direction of w_o<->(V=eye) 
//              by the surface material
//
// The integral is numerically solved.
// 
// BRDF                        : Bi-directional Reflectance Distribution Function
// The Cook-Torrence BRDF    : f_r = k_d * f_lambert + k_s * f_cook-torrence
//                            f_lambert = albedo / PI;
//
// about theory:
//  * <https://light11.hatenadiary.com/entry/2020/03/03/195249>
//
// about implementation:
//  * <https://light11.hatenadiary.com/entry/2020/03/05/220957>


#define PI 3.14159265359f

// constants
#define EPSILON 0.000000000001f

struct BRDFSurface
{
    float3 normal;
    float3 albedo;
    float roughness;
    float metallic;
};

// Trowbridge-Reitz GGX Distribution
// approximates the amount the surface's microfacets are aligned to the halfway vector, 
// influenced by the roughness of the surface; this is the primary function approximating the microfacets.
//
// NDF_GGXTR(N, H, roughness) = roughness^2 / ( PI * ( dot(N, H))^2 * (roughness^2 - 1) + 1 )^2
//
// details:
//  * <https://learnopengl.com/PBR/Theory>
//  * <http://reedbeta.com/blog/hows-the-ndf-really-defined/>
//
// @param normal
// @param wh halfway vector
inline float NormalDistributionGGX(float3 normal, float3 wh, float roughness)
{   
    const float a = roughness * roughness;
    const float a2 = a * a;
    const float nh2 = pow(max(dot(normal, wh), 0), 2);
    const float denom = (PI * pow((nh2 * (a2 - 1.0f) + 1.0f), 2)) + 0.0001f; // here 0.0001 for avoiding zero division

    return a2 / denom;
}

// Smith's Schlick-GGX for Direct Lighting (non-IBL)
// describes self shadowing of geometry
//
// G_ShclickGGX(N, V, k) = ( dot(N,V) ) / ( dot(N,V)*(1-k) + k )
//
// k         :    remapping of roughness based on wheter we're using geometry function 
//                for direct lighting or IBL
// k_direct     = (roughness + 1)^2 / 8
// k_IBL     = roughness^2 / 2
//
// @param w ray vector
inline float GeometrySmithSchlickGGX(float3 normal, float3 w, float roughness)
{
    const float k = pow((roughness + 1.0f), 2) / 8.0f;
    const float nv = max(0.0f, dot(normal, w));
    const float denom = (nv * (1.0f - k) + k) + 0.0001f;
    return nv / denom;
}


inline float GeometrySmith(float3 normal, float3 wi, float3 wo, float roughness)
{
    // essentially a multiplier [0, 1] measuring microfacet shadowing
    return GeometrySmithSchlickGGX(normal, wi, roughness) * GeometrySmithSchlickGGX(normal, wo, roughness);
}


// Fresnel-Schlick approximation describes reflection
inline float3 Fresnel(float3 wh, float3 wo, float3 f0)
{
    // F_Schlick(N, V, F0) = F0 - (1-F0)*(1 - dot(N,V))^5
    return f0 + (float3(1, 1, 1) - f0) * pow(1.0f - max(0.0f, dot(wh, wo)), 5.0f);
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

// about implementation:
// * <https://learnopengl.com/PBR/Theory>
float3 BRDF(BRDFSurface surface, float3 wi, float3 wo)
{
    const float3 normal = surface.normal;
    const float3 wh = normalize(wi + wo);
    
    //// surface
    const float3 albedo = surface.albedo;
    const float roughness = surface.roughness;
    const float metallic = surface.metallic;
    
    // As you can see, for non-metallic surfaces F0 is always 0.04. For metallic surfaces, 
    // we vary F0 by linearly interpolating between the original F0 and the albedo value given the metallic property.
    const float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    
    //// Fresnel_Cook-Torrence BRDF
    const float3 fresnel = Fresnel(wh, wo, f0);
    const float geometory = GeometrySmith(normal, wi, wo, roughness);
    const float ndf = NormalDistributionGGX(normal, wh, roughness);
    
    // Note that we add 0.0001 to the denominator to prevent a divide by zero in case any dot product ends up 0.0.
    const float denom = (4.0f * max(0.0f, dot(wo, normal)) * max(0.0f, dot(wi, normal))) + 0.0001f;
    
    const float3 specular = ndf * fresnel * geometory / denom;
    const float3 oSpecular = specular;
    
    // Now we can finally calculate each light's contribution to the reflectance equation. 
    // As the Fresnel value directly corresponds to kS we can use F to denote the specular 
    // contribution of any light that hits the surface. From kS we can then calculate the 
    // ratio of refraction kD:
    const float3 ks = fresnel;

    // Furthermore, because metallic surfaces don't refract light and thus have no diffuse 
    // reflections we enforce this property by nullifying kD if the surface is metallic.
    const float3 kd = (1.0 - ks) * (1.0f - metallic);
    
    // Diffuse BRDF
    const float3 oDiffuse = FLambertDiffuse(kd * albedo);
    
    return (oDiffuse + oSpecular);
}

float3 EnvironmentBRDF(BRDFSurface surface, float3 wi, float3 irradience, float3 envSpecular)
{
    // https://learnopengl.com/PBR/IBL/Specular-IBL
    const float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), surface.albedo, surface.metallic);
    const float3 ks = FresnelWithRoughness(max(dot(surface.normal, wi), 0.0), f0, surface.roughness);
    const float3 kd = (1.0f - ks) * (1.0 - surface.metallic);
    
    const float3 diffuse = irradience * surface.albedo;
    const float3 specular = envSpecular * ks;

    return (kd * diffuse + specular);
}