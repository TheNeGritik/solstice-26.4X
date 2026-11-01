#include <Features/Modules/Visual/ClickGui.hpp>
#include <Features/Modules/Visual/HudEditor.hpp>
#include "WndProcHook.hpp"

#include <Hook/Hooks/MiscHooks/KeyHook.hpp>
#include <Hook/Hooks/MiscHooks/MouseHook.hpp>

#include <windowsx.h> 

LRESULT CALLBACK WndProcHook::onDetour(HWND HWND, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (!ImGui::GetCurrentContext()) {
        return CallWindowProcW(mDetour, HWND, Msg, wParam, lParam);
    }

    if (Msg == WM_SETCURSOR)
    {
        auto clickGui = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<ClickGui>() : nullptr;
        auto hudEditor = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<HudEditor>() : nullptr;
        if ((clickGui && clickGui->mEnabled) || (hudEditor && hudEditor->mEnabled))
        {
            return TRUE;
        }
    }
    if (Msg == WM_KEYDOWN)
    {
        if (!(lParam & (1 << 30)))
        {
            KeyHook::onKey((uint32_t)wParam, true);
        }
    }
    else if (Msg == WM_KEYUP)
    {
        KeyHook::onKey((uint32_t)wParam, false);
    }

    switch (Msg) {/*stuck mpresskeys in winkey ;sob;*/
    case WM_ACTIVATEAPP: {
        if (wParam == FALSE) {
            Keyboard::mPressedKeys.clear();
        }
        break;
    }
    case WM_KILLFOCUS: {
        Keyboard::mPressedKeys.clear();
        break;
    }
    case WM_ACTIVATE: {
        if (LOWORD(wParam) == WA_INACTIVE) {
            Keyboard::mPressedKeys.clear();
        }
        break;
    }

    }

    ImGuiIO& io = ImGui::GetIO();

    switch (Msg) {
    case WM_MOUSEMOVE: {
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
        auto clickGui = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<ClickGui>() : nullptr;
        auto hudEditor = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<HudEditor>() : nullptr;
        if ((clickGui && clickGui->mEnabled) || (hudEditor && hudEditor->mEnabled))
        {
            return 0;
        }
        break;
    }

    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_MBUTTONUP:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    case WM_XBUTTONUP: {
        ImGuiMouseButton b = ImGuiMouseButton_Left;
        if (Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONDBLCLK || Msg == WM_RBUTTONUP) b = ImGuiMouseButton_Right;
        else if (Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONDBLCLK || Msg == WM_MBUTTONUP) b = ImGuiMouseButton_Middle;
        else if (Msg == WM_XBUTTONDOWN || Msg == WM_XBUTTONDBLCLK || Msg == WM_XBUTTONUP) b = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(b, (Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONDBLCLK ||Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONDBLCLK ||
            Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONDBLCLK ||Msg == WM_XBUTTONDOWN || Msg == WM_XBUTTONDBLCLK));
        auto clickGui = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<ClickGui>() : nullptr;
        auto hudEditor = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<HudEditor>() : nullptr;
        if ((clickGui && clickGui->mEnabled) || (hudEditor && hudEditor->mEnabled))
        {
            return 0;
        }
        break;
    }

    case WM_MOUSEWHEEL: {
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseWheelEvent(0.0f, ((float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA));
        auto clickGui = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<ClickGui>() : nullptr;
        auto hudEditor = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<HudEditor>() : nullptr;
        if ((clickGui && clickGui->mEnabled) || (hudEditor && hudEditor->mEnabled))
        {
            return 0;
        }
        break;
    }

    case WM_MOUSEHWHEEL: {
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseWheelEvent(-((float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA), 0.0f);
        auto clickGui = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<ClickGui>() : nullptr;
        auto hudEditor = (gFeatureManager && gFeatureManager->mModuleManager) ? gFeatureManager->mModuleManager->getModule<HudEditor>() : nullptr;
        if ((clickGui && clickGui->mEnabled) || (hudEditor && hudEditor->mEnabled))
        {
            return 0;
        }
        break;
    }
    case WM_SIZE:
    case WM_WINDOWPOSCHANGED:
    case WM_EXITSIZEMOVE:
    //case WM_DISPLAYCHANGE: //sometime bugging :joy:
    case WM_DPICHANGED: {/*fuck microsoft*/
        if (GetWindowLongPtr(HWND, GWLP_WNDPROC) != (LONG_PTR)onDetour) {
            mDetour = (WNDPROC)SetWindowLongPtr(HWND, GWLP_WNDPROC, (LONG_PTR)onDetour);
        }
        break;
    }
    }

    if ((Msg == WM_KEYUP || Msg == WM_KEYDOWN) && KeyHook::mCurrentCancel)
    {
        return 0;
    }

	return CallWindowProcW(mDetour, HWND, Msg, wParam, lParam);
}

void WndProcHook::init()
{
	std::thread([&] {
        while (!Solstice::mRequestEject)
        {
            if (Solstice::GameWindow && Solstice::mReadyWindow)
            {
                break;
            }

            Sleep(1);
        }
        if (Solstice::mRequestEject)
        {
            return;
        }

        mDetour = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(Solstice::GameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(onDetour)));
		}).detach();
}

void WndProcHook::shutdown()
{
    SetWindowLongPtrA(Solstice::GameWindow, GWLP_WNDPROC, (LONG_PTR)mDetour);
}
