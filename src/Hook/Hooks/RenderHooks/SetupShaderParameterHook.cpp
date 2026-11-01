//
// Created by vastrakai on 10/19/2024.
//

#include "SetupShaderParameterHook.hpp"
#include <Features/Events/SetupShaderParameterEvent.hpp>

std::unique_ptr<Detour> SetupShaderParameterHook::mDetour;

void SetupShaderParameterHook::SetupShaderParameter(class ScreenContext* screenContext, class BaseActorRenderContext* entityContext, class Actor* entity, 
    glm::vec4* overlay, glm::vec4* changeColor, glm::vec4* changeColor2, glm::vec4* glintColor, 
    float uvOffset1, float uvOffset2, float uvRot1, float uvRot2, glm::vec2* glintUVScale, glm::vec4* uvAnim, 
    float br, uint8_t lightEmission, std::optional<glm::vec3>& lightEmissionColor)
{
    auto original = mDetour->getOriginal<&SetupShaderParameter>();


    nes::event_holder<SetupShaderParameterEvent> holder = nes::make_holder<SetupShaderParameterEvent>(screenContext, entityContext, entity, overlay, changeColor, changeColor2, *glintColor, 
        uvOffset1, uvOffset2, uvRot1, uvRot2, glintUVScale, uvAnim, br, lightEmission, lightEmissionColor);
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCancelled()) return;
    MEMORY_BASIC_INFORMATION m1crosoftisr4tar3ded{};
    if (glintColor && VirtualQuery(glintColor, &m1crosoftisr4tar3ded, sizeof(m1crosoftisr4tar3ded)) && m1crosoftisr4tar3ded.State == MEM_COMMIT && !(m1crosoftisr4tar3ded.Protect & (PAGE_READONLY | PAGE_EXECUTE | PAGE_GUARD))) /*fuckyou clang or ms*/
    {
        *glintColor = holder->mglintColor;
    }
    original(screenContext, entityContext, entity, overlay, changeColor, changeColor2, glintColor,
        uvOffset1, uvOffset2, uvRot1, uvRot2, glintUVScale, uvAnim, br, lightEmission, lightEmissionColor);
}

void SetupShaderParameterHook::init()
{
    
    mDetour = std::make_unique<Detour>("ActorShaderManager::SetupShaderParameter", reinterpret_cast<void*>(SigManager::ActorShaderManager_setupShaderParameters), &SetupShaderParameter);
    mDetour->enable();
}
