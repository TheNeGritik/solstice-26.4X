#pragma once
// hiroki made it

#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>

class WndProcHook : public Hook {
public:
    WndProcHook() : Hook() {
        mName = "WndProc";
    }

    inline static WNDPROC mDetour;

    static LRESULT CALLBACK onDetour(HWND HWND, UINT Msg, WPARAM wParam, LPARAM lParam);
    void init() override;
    void shutdown() override;
};

