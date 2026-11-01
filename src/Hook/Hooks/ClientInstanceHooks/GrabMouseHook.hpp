#pragma once

#include <Hook/Hook.hpp>

class GrabMouseHook : public Hook {
public:
    GrabMouseHook() : Hook() {
        mName = "ClientInstance::grabMouse";
    }

    static std::unique_ptr<Detour> mDetour;
    inline static bool ShitNow;

    static void onGrabMouse(class ClientInstance* clientinstance);
    void init() override;
};

