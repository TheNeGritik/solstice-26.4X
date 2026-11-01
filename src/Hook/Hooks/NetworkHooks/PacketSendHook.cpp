//
// Created by vastrakai on 6/28/2024.
//
#include <SDK/Minecraft/ClientInstance.hpp>
#include <SDK/Minecraft/Network/LoopbackPacketSender.hpp>
#include <SDK/Minecraft/Network/Packets/TextPacket.hpp>
#include <SDK/Minecraft/Network/Packets/PlayerAuthInputPacket.hpp>
#include "PacketSendHook.hpp"
#include <Features/FeatureManager.hpp>
#include <SDK/Minecraft/Actor/Actor.hpp>
#include <SDK/Minecraft/World/Level.hpp>
#include "Features/Events/ChatEvent.hpp"
#include "Features/Events/PacketOutEvent.hpp"
#include <Utils/MemUtils.hpp>
#include <spdlog/spdlog.h>

std::unique_ptr<Detour> PacketSendHook::mDetour = nullptr;
std::unique_ptr<Detour> PacketSendHook::mSendToServerDetour = nullptr;
uint64_t PacketSendHook::mCurrentTick = 0;

static bool handleTextPacket(Packet* packet) {
    if (!packet || packet->getId() != PacketID::Text) return false;
    auto textPacket = reinterpret_cast<TextPacket*>(packet);
    if (!gFeatureManager || !gFeatureManager->mCommandManager) return false;

    std::string message = textPacket->getMessage();
    if (!message.empty() && message[0] == '.') {
        spdlog::info("PacketSendHook: Intercepted client command '{}'", message);
        auto event = nes::make_holder<ChatEvent>(message);
        gFeatureManager->mDispatcher->trigger(event);
        return true; // Always cancel so client commands never leak to server chat
    }
    return false;
}

void* PacketSendHook::onPacketSend(void* _this, Packet *packet) {
    auto original = mDetour->getOriginal<&onPacketSend>();

    if (handleTextPacket(packet)) return nullptr;

    if (packet->getId() == PacketID::PlayerAuthInput)
    {
        auto paip = reinterpret_cast<PlayerAuthInputPacket*>(packet);
        if (mApplyJump)
        {
            auto ci = ClientInstance::get();
            auto player = ci ? ci->getLocalPlayer() : nullptr;
            if (player) {
                if (Keyboard::isUsingMoveKeys()) paip->mInputData |= AuthInputAction::JUMPING | AuthInputAction::WANT_UP | AuthInputAction::JUMP_DOWN;
                if (!player->isOnGround() && player->wasOnGround() && Keyboard::isUsingMoveKeys()) {
                    paip->mInputData |= AuthInputAction::START_JUMPING;
                }
            }

            mApplyJump = false;
        }
    }

    auto holda = nes::make_holder<PacketOutEvent>(packet);
    gFeatureManager->mDispatcher->trigger(holda);
    if (holda->isCancelled()) return nullptr;

    if (packet->getId() == PacketID::PlayerAuthInput)
    {
        auto authPacket = reinterpret_cast<PlayerAuthInputPacket*>(packet);
        authPacket->mInteractRots = authPacket->mRot;
    }

    return original(_this, packet);
}

void* PacketSendHook::onPacketSendToServer(void* _this, Packet *packet) {
    if (mSendToServerDetour) {
        auto original = mSendToServerDetour->getOriginal<&onPacketSendToServer>();
        return original(_this, packet);
    }
    return nullptr;
}

void PacketSendHook::sendInputPacket(std::shared_ptr<PlayerAuthInputPacket> packet) {
    sendPacket(packet.get());
}

void PacketSendHook::sendPacket(PlayerAuthInputPacket* packet) {
    auto ci = ClientInstance::get();
    auto player = ci ? ci->getLocalPlayer() : nullptr;
    if (!player) return;

    auto sender = ci->getPacketSender();
    if (!sender) return;

    if (mCurrentTick == 0 && packet->mClientTick > mCurrentTick) mCurrentTick = packet->mClientTick;
    mCurrentTick++;

    packet->mClientTick = mCurrentTick;
    if (player->getLevel() && player->getLevel()->getLevelData()) {
        player->getLevel()->getLevelData()->mTick = mCurrentTick;
    }

    sender->sendToServer(packet);
}

void PacketSendHook::init() {
    auto ci = ClientInstance::get();
    if (!ci) return;
    auto sender = ci->getPacketSender();
    if (!sender || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(sender))) return;
    auto vtable = sender->vtable;
    if (!vtable || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(vtable))) return;

    if (!mDetour) {
        void* sendFunc = vtable[2];
        if (sendFunc && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(sendFunc))) {
            mDetour = std::make_unique<Detour>("LoopbackPacketSender::send", sendFunc, &PacketSendHook::onPacketSend);
            mDetour->enable();
            spdlog::info("PacketSendHook: Hooked LoopbackPacketSender::send at {:p}", sendFunc);
        }
    }
}
