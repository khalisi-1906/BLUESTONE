💎 Gemstone Rendering Enhancements (HLSL/C++ Optics Module)

Project Overview

This repository contains the core logic developed during the summer internship at Blue Stone Tech Labs Pvt. Ltd. The project's goal was to implement advanced, physically-based optical effects on transparent materials within the Rhino CAD rendering pipeline, specifically targeting high-fidelity jewelry visualization. The implementation uses HLSL for GPU rendering logic and C++ (with ImGui) for application-level control.

✨ Features Implemented

The following features enhance the visual realism and artistic control over gemstone rendering:

1. Chromatic Dispersion (Abbe Value)

Simulates the "fire" effect by calculating wavelength-dependent Index of Refraction (IOR) values, splitting light into red, green, and blue components upon refraction.

2. Intersecting Geometry Control

A multi-pass rendering strategy that manipulates the Depth Buffer to ensure transparent objects (like diamonds) are rendered fully visible, even when intersecting with opaque geometry (like metal prongs), thus avoiding unnecessary occlusion and reflection artifacts.

3. User-Friendly Master Toggle

A unified GUI control allowing designers to instantly enable or disable both advanced optical features, streamlining the workflow.

🛠️ Technical Implementation Details

A. HLSL Shader Logic: Chromatic Dispersion

The dispersion effect is achieved by calculating three distinct IOR ratios based on the material's defined Abbe Value.

Core Logic in mainPixelShader:

// Constant Buffer inputs
cbuffer MaterialConstants : register(b0)
{
    float gemstoneIOR;    // Base IOR
    float gemstoneAbbe;   // Abbe value (VD)
};

// 1. Calculate the dispersion offset (Delta)
float deltaIOR = (gemstoneIOR - 1.0f) / gemstoneAbbe; 

// 2. Compute IORs for R, G, B channels
float ior_red   = gemstoneIOR - deltaIOR;
float ior_blue  = gemstoneIOR + deltaIOR;

// 3. Refract view direction using the three specific IORs
float3 refractedRay_R = RefractRay(-V, N, 1.0f / ior_red);
float3 refractedRay_B = RefractRay(-V, N, 1.0f / ior_blue);
// ... RefractedRay_G uses gemstoneIOR ...

// 4. Sample and combine the color channels from the environment map
float R = environmentMap.Sample(envSampler, refractedRay_R).r;
float G = environmentMap.Sample(envSampler, refractedRay_G).g;
float B = environmentMap.Sample(envSampler, refractedRay_B).b;

float4 finalColor = float4(R, G, B, 1.0f);


B. C++ Application Logic: Master Feature Toggle

The C++ host application manages a state variable (g_areAdvancedOpticalFeaturesEnabled) which dictates the rendering path. This variable is controlled via an ImGui Checkbox.

C++ ImGui Toggle Snippet:

// State Variable
bool g_areAdvancedOpticalFeaturesEnabled = true; 

void RenderGemstoneFeatureToggle()
{
    ImGui::Begin("Jewelry Rendering Controls"); 
    ImGui::Text("Render Enhancements (Optics)");
    ImGui::Separator();
    
    // Checkbox controls the state of g_areAdvancedOpticalFeaturesEnabled
    ImGui::Checkbox("Enable Advanced Optical Features", &g_areAdvancedOpticalFeaturesEnabled);
    ImGui::SameLine(); 
    ImGui::TextDisabled("(Dispersion & Intersection Control)"); 

    ImGui::End();
}


C++ Rendering Pipeline Integration (Conceptual):

The boolean state controls the execution of the multi-pass rendering sequence:

void ExecuteRenderPasses()
{
    // ... Render Opaque objects (Pass 1) ...
    
    if (g_areAdvancedOpticalFeaturesEnabled)
    {
        // Advanced Mode: Enables HLSL Abbe calculations, 
        // and sets GPU state: glDisable(GL_DEPTH_TEST), glDepthMask(GL_FALSE) 
        RenderTransparentPass_Advanced();
    }
    else
    {
        // Legacy Mode: Standard single-IOR refraction and default depth/blend states
        RenderTransparentPass_Legacy();
    }
}


🔗 Source Control Reference

NOTE: Due to proprietary and confidentiality agreements, the source code for this project cannot be made public (as per Clause 13 in the Offer Letter).

Internal Repository Location:
The complete, implemented source code is stored on Blue Stone Tech Labs Pvt. Ltd.'s internal version control system.

Project Path: //depot/rhino-renderer/features/gemstone_optics

Final Commit Hash/Branch: [INSERT FINAL COMMIT HASH OR BRANCH NAME HERE]

Access: Access restricted to internal employees and management.

Academic Submission Note (If applicable):
For external academic review, a sanitized copy of the HLSL and C++ files, stripped of proprietary framework dependencies, is provided under separate confidential cover.

📝 Usage and Integration

Material Setup: Ensure all gemstone materials are assigned gemstoneIOR and gemstoneAbbe values in the material constants buffer.

Toggle Control: Use the "Enable Advanced Optical Features" checkbox in the GUI window to dynamically switch between the photorealistic and legacy rendering modes.

Performance: Note that enabling this feature requires reading multiple color channels and potentially two rendering passes, resulting in a marginal increase in GPU workload compared to the legacy single-IOR refraction model.
