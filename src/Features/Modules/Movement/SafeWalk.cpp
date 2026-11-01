//
// Created by vastrakai on 7/18/2024.
//

#include "SafeWalk.hpp"

#include <Features/Events/BaseTickEvent.hpp>
//pasted from na tu re :joy: but i made this :ultra_retard:

unsigned char Bytes1[] = {
    0xC6, 0x40, 0x55, 0x01,
    0xE9, 0x6F, 0, 0, 0,
    0x90, 0x90, 0x90, 0x90 
};
unsigned char Bytes2[] = {
    0xC6, 0x40, 0x55, 0x01,
    0xE9, 0, 0, 0, 0,
    0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90 
};


static void patchSafeWalk(bool v) {
    static bool current = false;
    static uintptr_t adr = SigManager::SneakMovementSystem_tickSneakMovementSystem;
    static uintptr_t adr2 = SigManager::SneakMovementSystem_tickSneakMovementSystem2;
    if (current == v) {
        return;
    }
    if (v) {
        MemUtils::ReadBytes((void*)adr, SafeWalk::mOriginalData, sizeof(SafeWalk::mOriginalData));
        MemUtils::ReadBytes((void*)adr2, SafeWalk::mOriginalData2, sizeof(SafeWalk::mOriginalData2));

        *(int32_t*)(Bytes1 + 5) = (int32_t)((adr + 0x78) - (adr + 9));
        MemUtils::writeBytes(adr, Bytes1, sizeof(Bytes1));

        *(int32_t*)(Bytes2 + 5) = (int32_t)(((uintptr_t)adr2 + 0x19) - ((uintptr_t)adr2 + 9));
        MemUtils::writeBytes(adr2, Bytes2, sizeof(Bytes2));
        current = v;


    }
    else {
        MemUtils::writeBytes(adr, SafeWalk::mOriginalData, sizeof(SafeWalk::mOriginalData));
        MemUtils::writeBytes(adr2, SafeWalk::mOriginalData, sizeof(SafeWalk::mOriginalData));
        current = v;

    }

}

void SafeWalk::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &SafeWalk::onBaseTickEvent>(this);
}

void SafeWalk::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &SafeWalk::onBaseTickEvent>(this);
    patchSafeWalk(false);

}

void SafeWalk::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (player->isOnGround()) {
        patchSafeWalk(mEnabled);
    }
    else {
        patchSafeWalk(false);
    }

}
