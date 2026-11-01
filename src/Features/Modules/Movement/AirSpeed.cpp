//
// Created by alteik on 04/10/2024.
//

#include "AirSpeed.hpp"
#include <SDK/SigManager.hpp>

void AirSpeed::onInit() {
    uintptr_t addr = SigManager::GetSpeedInAirWithSprint;
    int32_t rel; 
    memcpy(&rel, (void*)(addr + 0x13), 4);
    uintptr_t tb = (addr + 0x17) + rel;

    mSpeedInAirWithoutSprintPtr = reinterpret_cast<float*>(tb + 4);
    mSpeedInAirWithSprintPtr = reinterpret_cast<float*>(tb + 0);
}

void AirSpeed::onEnable() {
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &AirSpeed::onBaseTickEvent>(this);
}

void AirSpeed::onDisable() {
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &AirSpeed::onBaseTickEvent>(this);
    MemUtils::Write((uintptr_t) mSpeedInAirWithoutSprintPtr, mSpeedInAirWithoutSprintValue);
    MemUtils::Write((uintptr_t) mSpeedInAirWithSprintPtr, mSpeedInAirWithSprintValue);
}

void AirSpeed::onBaseTickEvent(BaseTickEvent &event) {
    MemUtils::Write((uintptr_t) mSpeedInAirWithoutSprintPtr, mSpeedInAirWithoutSprintValue * mNoSprintSpeed.mValue);
    MemUtils::Write((uintptr_t) mSpeedInAirWithSprintPtr, mSpeedInAirWithSprintValue * mSprintSpeed.mValue);
}