#pragma once
//
// Created by vastrakai on 7/8/2024.
//

#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>


class LevelRenderer;
class ScreenContext;

class LevelRendererHook : public Hook {
public:
    LevelRendererHook() : Hook() {
        mName = "LevelRenderer::renderLevel";
    }

    static std::unique_ptr<Detour> mDetour;
    static inline LevelRenderer* mInstance = nullptr;

    static void renderLevel(LevelRenderer* _this, ScreenContext* screenContext, void* a3);
    void init() override;
};
