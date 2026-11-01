#pragma once
//
// Created by vastrakai on 10/19/2024.
//

#include <Hook/Hook.hpp>

class SetupShaderParameterHook : public Hook {
public:
    SetupShaderParameterHook() : Hook() {
        mName = "SetupShaderParameterHook";
    }

    static std::unique_ptr<Detour> mDetour;
    static void SetupShaderParameter(class ScreenContext* screenContext, class BaseActorRenderContext* entityContext, class Actor* entity,glm::vec4* overlay, glm::vec4* changeColor, glm::vec4* changeColor2, glm::vec4* glintColor,
        float uvOffset1, float uvOffset2, float uvRot1, float uvRot2, glm::vec2* glintUVScale, glm::vec4* uvAnim, float br, uint8_t lightEmission, std::optional<glm::vec3>& lightEmissionColor);
    void init() override;
};