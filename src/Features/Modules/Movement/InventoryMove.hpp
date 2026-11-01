#pragma once
//
// Created by vastrakai on 7/12/2024.
//

#include <Features/Modules/Module.hpp>

class InventoryMove : public ModuleBase<InventoryMove> {
public:
    BoolSetting mDisallowShift = BoolSetting("Disallow Sneaking", "Disallows sneaking while in inventory.", false);

    InventoryMove() : ModuleBase("InventoryMove", "Allows you to move while your inventory is open.", ModuleCategory::Movement, 0, false) {
        addSettings(&mDisallowShift);

        mNames = {
            {Lowercase, "inventorymove"},
            {LowercaseSpaced, "inventory move"},
            {Normal, "InventoryMove"},
            {NormalSpaced, "Inventory Move"}
        };
    }

    bool mHasOpenContainer = false;

    std::vector<unsigned char> OriginalBytes;
    std::vector<unsigned char> OriginalBytes2;
    std::vector<unsigned char> OriginalBytes3;
    bool Patched = false;


    void Patch();
    void UnPatch();

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void onPacketInEvent(class PacketInEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
};