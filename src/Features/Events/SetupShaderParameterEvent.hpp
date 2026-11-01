//
// Created by vastrakai on 10/19/2024.
//

#pragma once

#include "Event.hpp"

class SetupShaderParameterEvent : public CancelableEvent {
public:
    SetupShaderParameterEvent(class ScreenContext* screenContext, class BaseActorRenderContext* entityContext, class Actor* entity,
        glm::vec4* overlay, glm::vec4* changeColor, glm::vec4* changeColor2, glm::vec4 glintColor,
        float uvOffset1, float uvOffset2, float uvRot1, float uvRot2, glm::vec2* glintUVScale, glm::vec4* uvAnim,
        float br, uint8_t lightEmission, std::optional<glm::vec3>& lightEmissionColor)
        : mscreenContext(screenContext), mentityContext(entityContext), mentity(entity), moverlay(overlay), mchangeColor(changeColor)
        , mchangeColor2(changeColor2), mglintColor(glintColor), muvOffset1(uvOffset1), muvOffset2(uvOffset2), muvRot1(uvRot1), muvRot2(uvRot2)
        , mglintUVScale(glintUVScale), mbr(br), mlightEmission(lightEmission), mlightEmissionColor(lightEmissionColor) {}

    class ScreenContext* mscreenContext;
    class BaseActorRenderContext* mentityContext;
    class Actor* mentity;
    glm::vec4* moverlay;
    glm::vec4* mchangeColor;
    glm::vec4* mchangeColor2;
    glm::vec4 mglintColor;
    float muvOffset1;
    float muvOffset2;
    float muvRot1;
    float muvRot2;
    glm::vec2* mglintUVScale;
    glm::vec4* muvAnim;
    float mbr;
    uint8_t mlightEmission;
    std::optional<glm::vec3>& mlightEmissionColor;
};
