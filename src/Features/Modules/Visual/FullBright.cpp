//
// Created by vastrakai on 9/13/2024.
//

#include "FullBright.hpp"
#include <SDK/SigManager.hpp>
#include <spdlog/spdlog.h>

void FullBright::onInit()
{
    if (SigManager::Options_getGamma)
    {
        mDetour = std::make_unique<Detour>("Options::getGamma", reinterpret_cast<void*>(SigManager::Options_getGamma), &FullBright::onGetGamma);
        mDetour->enable();
        spdlog::info("[FullBright] Hooked Options::getGamma at {:X}", SigManager::Options_getGamma);
    }
    else
    {
        spdlog::error("[FullBright] Options_getGamma signature not found!");
    }
}

float FullBright::onGetGamma(void* _this)
{
    auto orig = mDetour->getOriginal<&onGetGamma>();
    if (mInstance && mInstance->mEnabled)
    {
        return mInstance->mGamma.as<float>();
    }
    return orig(_this);
}
