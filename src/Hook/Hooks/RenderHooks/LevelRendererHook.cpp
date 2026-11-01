#include "LevelRendererHook.hpp"
#include <Features/Events/LevelRenderEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>

#include "D3DHook.hpp"

std::unique_ptr<Detour> LevelRendererHook::mDetour;

void LevelRendererHook::renderLevel(LevelRenderer* _this, ScreenContext* screenContext, void* a3)
{
    if (_this && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(_this))) {
        mInstance = _this;
    }
    auto oFunc = mDetour->getOriginal<&renderLevel>();

    auto holder1 = nes::make_holder<LevelRenderEvent>(false);
    gFeatureManager->mDispatcher->trigger(holder1);

    oFunc(_this, screenContext, a3);

    auto holder2 = nes::make_holder<LevelRenderEvent>(true);
    gFeatureManager->mDispatcher->trigger(holder2);
}

void LevelRendererHook::init()
{
    mDetour = std::make_unique<Detour>("LevelRenderer::renderLevel", reinterpret_cast<void*>(SigManager::LevelRenderer_renderLevel), &LevelRendererHook::renderLevel);
}
