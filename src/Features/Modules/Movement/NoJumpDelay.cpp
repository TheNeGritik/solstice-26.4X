//
// Created by vastrakai on 7/19/2024.
//

#include "NoJumpDelay.hpp"

#include <Features/Events/BaseTickEvent.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>


std::vector<unsigned char> Bytes = { 0x0 };
DEFINE_PATCH_FUNC(PatchNoJumpDelay_impl, SigManager::Mob_JumpFromGroundRequestComponentPatch + 3, Bytes); // fuck microsoft

void NoJumpDelay::PatchNoJumpDelay(bool v) {
    static bool current = false;
    if (current != v) {
        PatchNoJumpDelay_impl(v);
        current = v;
    }
}

void NoJumpDelay::onEnable()
{
    gFeatureManager->mDispatcher->listen<BaseTickEvent, &NoJumpDelay::onBaseTickEvent>(this);

    const auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    PatchNoJumpDelay(true);
}

void NoJumpDelay::onDisable()
{
    gFeatureManager->mDispatcher->deafen<BaseTickEvent, &NoJumpDelay::onBaseTickEvent>(this);

    const auto player = ClientInstance::get()->getLocalPlayer();
    if (!player) return;

    PatchNoJumpDelay(false);
}

void NoJumpDelay::onBaseTickEvent(BaseTickEvent& event)
{
    auto player = event.mActor;
    if (!player) return;
    PatchNoJumpDelay(true);
}
