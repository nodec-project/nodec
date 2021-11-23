

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
inline float NormalDistributionGGX(float3 normal, float3 wh, float roughness)
{
    // approximates microfacets :    approximates the amount the surface's microfacets are
    //                                aligned to the halfway vector influenced by the roughness
    //                                of the surface
    //                            :    determines the size, brightness, and shape of the specular highlight
    // more: http://reedbeta.com/blog/hows-the-ndf-really-defined/
    //
    // NDF_GGXTR(N, H, roughness) = roughness^2 / ( PI * ( dot(N, H))^2 * (roughness^2 - 1) + 1 )^2
    const float a = roughness * roughness;
    const float a2 = a * a;
    const float nh2 = pow(max(dot(normal, wh), 0), 2);
    const float denom = (PI * pow((nh2 * (a2 - 1.0f) + 1.0f), 2));
    if (denom < EPSILON)
        return 1.0f;

    return a2 / denom;
}

// Smith's Schlick-GGX for Direct Lighting (non-IBL)
inline float GeometrySmithsSchlickGGX(float3 normal, float3 w, float roughness)
{
    // describes self shadowing of geometry
    //
    // G_ShclickGGX(N, V, k) = ( dot(N,V) ) / ( dot(N,V)*(1-k) + k )
    //
    // k         :    remapping of roughness based on wheter we're using geometry function 
    //                for direct lighting or IBL
    // k_direct     = (roughness + 1)^2 / 8
    // k_IBL     = roughness^2 / 2
    //
    const float k = pow((roughness + 1.0f), 2) / 8.0f;
    const float nv = max(0.0f, dot(normal, w));
    const float denom = (nv * (1.0f - k) + k) + 0.0001f;
    if (denom < EPSILON)
        return 1.0f;
    return nv / denom;
}

inline float Geometry(float3 normal, float3 wi, float3 wo, float roughness)
{
    // essentially a multiplier [0, 1] measuring microfacet shadowing
    return GeometrySmithsSchlickGGX(normal, wi, roughness) * GeometrySmithsSchlickGGX(normal, wo, roughness);
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

// @note
// <https://learnopengl.com/PBR/Theory>
float3 BRDF(BRDFSurface surface, float3 wi, float3 wo)
{
    const float3 normal = surface.normal;
    const float3 wh = normalize(wi + wo);
    
    //// surface
    const float3 albedo = surface.albedo;
    const float roughness = surface.roughness;
    const float metallic = surface.metallic;
    const float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);
    
    //// Fresnel_Cook-Torrence BRDF
    const float3 fresnel = Fresnel(wh, wo, f0);
    const float geometory = Geometry(normal, wi, wo, roughness);
    const float ndf = NormalDistributionGGX(normal, wh, roughness);
    const float denom = (4.0f * max(0.0f, dot(wo, normal)) * max(0.0f, dot(wi, normal))) + 0.0001f;
    const float3 specular = ndf * fresnel * geometory / denom;
    const float3 oSpecular = specular;
    
    // Diffuse BRDF
    const float3 ks = fresnel;
    const float3 kd = (float3(1.0f, 1.0f, 1.0f) - ks) * (1.0f - metallic) * albedo;
    const float3 oDiffuse = FLambertDiffuse(kd);
    
    return (oDiffuse + oSpecular);
}

float3 EnvironmentBRDF(BRDFSurface surface, float3 viewDir, float3 irradience, float3 envSpecular)
{
    const float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), surface.albedo, surface.metallic);
    const float3 ks = FresnelWithRoughness(max(dot(surface.normal, viewDir), 0.0), f0, surface.roughness);
    const float kd = (1.0f - ks) * (1.0f - surface.metallic);
    
    const float3 diffuse = irradience * surface.albedo;
    const float3 specular = envSpecular * ks;

    return (kd * diffuse + specular);
}