#pragma once
//
// Created by vastrakai on 7/5/2024.
//

#include <Utils/MemUtils.hpp>
#include <SDK/SigManager.hpp>


class GameMode {
public:
    CLASS_FIELD(class Actor*, mPlayer, 0x8);
    CLASS_FIELD(float, mBreakProgress, 0x24);
    bool startDestroyBlock(glm::ivec3 pos, unsigned char face, bool hasDestroyedBlock)
    {
        return MemUtils::callVirtualFunc<bool, glm::ivec3, unsigned char, bool&>(1, this, pos, face, hasDestroyedBlock);
    }
    bool destroyBlock(glm::ivec3 pos, unsigned char face)
    {
        return MemUtils::callVirtualFunc<bool, glm::ivec3, unsigned char>(2, this, pos, face);
    }
    bool stopDestroyBlock(glm::ivec3 pos)
    {
        return MemUtils::callVirtualFunc<bool, glm::ivec3>(4, this, pos);
    }
    bool buildBlock(glm::ivec3 pos, unsigned char face, unsigned char slot /*HAND SLOT*/, bool issimtick)
    {
        return MemUtils::callVirtualFunc<bool, glm::ivec3, unsigned char, unsigned char, bool>(OffsetProvider::GameMode_buildBlock, this, pos, face, slot, issimtick);
    }
    bool interact(Actor* Entity, glm::vec3 Location, unsigned char slot /*HAND SLOT*/)
    {
        return MemUtils::callVirtualFunc<bool, Actor*, glm::vec3, unsigned char>(15, this, Entity, Location, slot);
    }
    bool attack(Actor& Entity, glm::vec3 AttackPos = glm::vec3(0.f, 0.f, 0.f))
    {
        if (AttackPos.x == 0.f && AttackPos.y == 0.f && AttackPos.z == 0.f)
        {
            glm::vec3* pPos = Entity.getPos();
            if (pPos) AttackPos = *pPos + glm::vec3(0.f, 1.f, 0.f);
        }
        MemUtils::callVirtualFunc<bool, Actor&, glm::vec3>(14, this, Entity, AttackPos);
        return MemUtils::callVirtualFunc<bool, Actor&, glm::vec3>(15, this, Entity, AttackPos);
    }
    void releaseUsingItem()
    {
        MemUtils::callVirtualFunc<void>(16, this);
    }

    float getDestroyRate(const class Block& block);
    bool baseUseItem(class ItemStack* itemStack);
};