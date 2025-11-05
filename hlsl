// Constant Buffer matching the C++ data layout (or individual uniforms)
cbuffer MaterialConstants : register(b0)
{
    float gemstoneIOR;    // Base IOR (nD for green light)
    float gemstoneAbbe;   // Abbe value (VD)
};

// Texture/Sampler for Environment Mapping (required for refraction color)
TextureCube environmentMap;
SamplerState envSampler;

// Function to calculate refraction vector (Snell's Law implementation)
// Note: HLSL's built-in refract function can be used, but this shows the principle.
float3 RefractRay(float3 viewDir, float3 normal, float iorRatio)
{
    // iorRatio = ior_incoming / ior_material (e.g., 1.0 / IOR_red) [cite: 728]
    float cosTheta = dot(viewDir, normal);
    float sinTheta2 = iorRatio * iorRatio * (1.0f - cosTheta * cosTheta);
    
    // Check for Total Internal Reflection (TIR) [cite: 700, 702]
    if (sinTheta2 > 1.0f)
    {
        return reflect(viewDir, normal); // Return reflection instead of refraction [cite: 148]
    }
    
    // Calculate refracted ray (using HLSL's built-in refract or the full formula)
    return iorRatio * viewDir + (iorRatio * cosTheta - sqrt(1.0f - sinTheta2)) * normal; [cite: 149]
}

// Main logic to compute the dispersed color
float4 mainPixelShader(float3 WorldPosition, float3 WorldNormal, float3 ViewDirection) : SV_Target
{
    // 1. Compute dispersion offset (Delta n)
    // Formula derived from Abbe number: Delta n = (nD - 1) / VD
    // We use this Delta to approximate IOR variation [cite: 792]
    float deltaIOR = (gemstoneIOR - 1.0f) / gemstoneAbbe; 

    // 2. Compute IOR for R, G, B channels [cite: 793, 794]
    float ior_red   = gemstoneIOR - deltaIOR;
    float ior_green = gemstoneIOR;
    float ior_blue  = gemstoneIOR + deltaIOR;

    // The ratio of IORs (Air IOR / Material IOR) is needed for refraction [cite: 728]
    float ratio_red   = 1.0f / ior_red;
    float ratio_green = 1.0f / ior_green;
    float ratio_blue  = 1.0f / ior_blue;

    // 3. Refract view direction using each IOR ratio [cite: 795]
    // The view direction must be pointing *into* the surface (negative ray direction)
    float3 V = normalize(ViewDirection);
    float3 N = normalize(WorldNormal);
    
    float3 refractedRay_R = RefractRay(-V, N, ratio_red);
    float3 refractedRay_G = RefractRay(-V, N, ratio_green);
    float3 refractedRay_B = RefractRay(-V, N, ratio_blue);

    // 4. Sample color from cubemap using each ray [cite: 796]
    // Sample the red channel of the cubemap at the red-refracted direction
    float R = environmentMap.Sample(envSampler, refractedRay_R).r;
    // Sample the green channel of the cubemap at the green-refracted direction
    float G = environmentMap.Sample(envSampler, refractedRay_G).g;
    // Sample the blue channel of the cubemap at the blue-refracted direction
    float B = environmentMap.Sample(envSampler, refractedRay_B).b;

    // 5. Output color with simulated dispersion [cite: 797]
    float4 finalColor = float4(R, G, B, 1.0f);
    
    // [Apply shading, Fresnel, etc. here for a complete PBR implementation]
    
    return finalColor;
}
