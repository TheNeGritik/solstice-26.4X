//
// Created by vastrakai on 6/28/2024.
//

#include "LoopbackPacketSender.hpp"
#include <Hook/Hooks/NetworkHooks/PacketSendHook.hpp>
#include <SDK/Minecraft/Network/Packets/Packet.hpp>

void LoopbackPacketSender::sendToServer(void* packet) {
    if (PacketSendHook::mDetour) {
        auto original = PacketSendHook::mDetour->getOriginal<&PacketSendHook::onPacketSend>();
        original(this, reinterpret_cast<Packet*>(packet));
    } else if (vtable && vtable[2]) {
        using SendFn = void* (*)(void*, void*);
        reinterpret_cast<SendFn>(vtable[2])(this, packet);
    }
}
