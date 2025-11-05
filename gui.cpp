#include "imgui.h" 
// Include the ImGui header

// 1. Define the Global State Variable
// This boolean will hold the ON/OFF state of the features.
// It should be part of a global settings struct or application state.
bool g_areAdvancedOpticalFeaturesEnabled = true; 

// 2. GUI Rendering Function (Called in the main render loop)
void RenderGemstoneFeatureToggle()
{
    // Begin a new ImGui window. You might integrate this into an existing Material Properties window.
    ImGui::Begin("Jewelry Rendering Controls"); 

    // --- The Master Toggle Implementation ---
    ImGui::Text("Render Enhancements (Optics)");
    ImGui::Separator();
    
    // ImGui::Checkbox links the UI element to the C++ boolean variable.
    // Clicking the checkbox updates the value of g_areAdvancedOpticalFeaturesEnabled.
    bool wasEnabled = g_areAdvancedOpticalFeaturesEnabled;
    ImGui::Checkbox("Enable Advanced Optical Features", &g_areAdvancedOpticalFeaturesEnabled);
    
    // Optional: Add a label on the same line for clarity
    ImGui::SameLine(); 
    ImGui::TextDisabled("(Dispersion & Intersection Control)"); 

    // Optional: Provide visual feedback/context when enabled
    if (g_areAdvancedOpticalFeaturesEnabled)
    {
        ImGui::Spacing();
        ImGui::Text("Status: Active. Rendering with Abbe Dispersion and Depth Management.");
    }
    else if (wasEnabled) // Only show this message briefly after turning OFF
    {
        // This logic can be removed in production but helps during debugging
        ImGui::Text("Status: Disabled. Using standard/legacy rendering mode.");
    }

    ImGui::End();
}

// 3. Conceptual Use in the Rendering Pipeline
void ExecuteRenderPasses()
{
    // ... Render Opaque objects ...
    
    // Check the GUI state before rendering transparent objects
    if (g_areAdvancedOpticalFeaturesEnabled)
    {
        // Execute the advanced, multi-pass rendering logic:
        // 1. Set shader uniforms: pass Abbe values (IOR_red, IOR_green, IOR_blue).
        // 2. Set API state: glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
        
        [cite_start]// This enables: Dispersion [cite: 944][cite_start], Ignoring intersecting geometry [cite: 945]
        RenderTransparentPass_Advanced();
    }
    else
    {
        // Execute the standard/legacy rendering path
        RenderTransparentPass_Legacy();
    }
}
