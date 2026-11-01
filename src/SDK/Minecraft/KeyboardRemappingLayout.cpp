//
// Created by vastrakai on 10/10/2024.
//

#include "KeyboardRemappingLayout.hpp"
#include <Windows.h>

int KeyboardRemappingLayout::operator[](const std::string& key)
{
    if (this) {
        __try {
            for (auto& bind : mKeyTypeA) {
                if (bind.mBindName == key) {
                    if (!bind.mBindKey.empty())
                        return bind.mBindKey[0];
                    break;
                }
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    // Default Minecraft keybindings fallback
    if (key == "key.forward") return 'W';
    if (key == "key.left") return 'A';
    if (key == "key.back") return 'S';
    if (key == "key.right") return 'D';
    if (key == "key.jump") return VK_SPACE;
    if (key == "key.sneak") return VK_SHIFT;
    if (key == "key.sprint") return VK_CONTROL;

    return -1;
}

KeyboardRemappingLayout* ClientInputMappingFactory::getKeyboardRemappingLayout()
{
    if (!this) return nullptr;
    __try {
        return hat::member_at<KeyboardRemappingLayout*>(this, OffsetProvider::ClientInputMappingFactory_mKeyboardRemappingLayout);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

ClientInputMappingFactory* ClientInputHandler::getMappingFactory()
{
    if (!this) return nullptr;
    __try {
        return hat::member_at<ClientInputMappingFactory*>(this, OffsetProvider::ClientInputHandler_mMappingFactory);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}
