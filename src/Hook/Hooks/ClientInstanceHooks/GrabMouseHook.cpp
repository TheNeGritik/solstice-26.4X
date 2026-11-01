#include "GrabMouseHook.hpp"

#include <SDK/Minecraft/ClientInstance.hpp>

std::unique_ptr<Detour> GrabMouseHook::mDetour = nullptr;

void GrabMouseHook::onGrabMouse(ClientInstance* clientinstance)
{
    auto oFunc = mDetour->getOriginal<&onGrabMouse>();
    
    if (ShitNow)
    {
        return;
    }

    return oFunc(clientinstance);
}

void GrabMouseHook::init()
{
    auto ci = ClientInstance::get();
    if (!ci || !ci->vtable) return;
    mDetour = std::make_unique<Detour>("ClientInstance::grabMouse", reinterpret_cast<void*>(ci->vtable[OffsetProvider::ClientInstance_grabMouse]), &GrabMouseHook::onGrabMouse);
    mDetour->enable();
}
