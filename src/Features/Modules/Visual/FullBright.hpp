#pragma once
//
// Created by vastrakai on 9/13/2024.
//

#include <Features/Modules/Module.hpp>
#include <Features/Modules/Setting.hpp>
#include <Hook/Detour.hpp>
#include <memory>

class FullBright : public ModuleBase<FullBright>
{
public:
    static inline FullBright* mInstance = nullptr;
    static inline std::unique_ptr<Detour> mDetour = nullptr;

    NumberSetting mGamma = NumberSetting("Gamma", "The brightness value", 20.0f, 1.0f, 50.0f, 1.0f);

    FullBright() : ModuleBase("FullBright", "Gives your game full brightness", ModuleCategory::Visual, 0, false) {
        mInstance = this;
        addSettings(&mGamma);

        mNames = {
            {Lowercase, "fullbright"},
            {LowercaseSpaced, "full bright"},
            {Normal, "FullBright"},
            {NormalSpaced, "Full Bright"}
        };
    }

    void onInit() override;
    static float onGetGamma(void* _this);
};
