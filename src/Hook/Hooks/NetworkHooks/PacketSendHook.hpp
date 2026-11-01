#pragma once
//
// Created by vastrakai on 6/28/2024.
//

#include <Hook/Hook.hpp>

class PacketSendHook : public Hook {
public:
    PacketSendHook() : Hook() {
        mName = "LoopbackPacketSender::send";
    }

    static std::unique_ptr<Detour> mDetour;
    static std::unique_ptr<Detour> mSendToServerDetour;
    static uint64_t mCurrentTick;
    static inline bool mApplyJump = false;

    static void* onPacketSend(void* _this, class Packet* packet);
    static void* onPacketSendToServer(void* _this, class Packet* packet);
    static void sendInputPacket(std::shared_ptr<class PlayerAuthInputPacket> packet);
    static void sendPacket(class PlayerAuthInputPacket* packet);
    void init() override;
};
