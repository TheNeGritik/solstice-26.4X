//
// Created by vastrakai on 6/25/2024.
//

#include "MinecraftSim.hpp"

#include <libhat/Access.hpp>
#include <SDK/OffsetProvider.hpp>
#include <Windows.h>

static bool isMemoryWritable(void* ptr, size_t size = sizeof(float))
{
    if (!ptr || reinterpret_cast<uintptr_t>(ptr) < 0x10000) return false;
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if ((mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) == 0) return false;
    if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) return false;
    return true;
}

static bool isMemoryReadable(const void* ptr, size_t size = sizeof(float))
{
    if (!ptr || reinterpret_cast<uintptr_t>(ptr) < 0x10000) return false;
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) return false;
    return true;
}

Simulation* MinecraftSim::getGameSim()
{
    if (!this) return nullptr;
    __try {
        return hat::member_at<Simulation*>(this, OffsetProvider::MinecraftSim_mGameSim);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

Simulation* MinecraftSim::getRenderSim()
{
    if (!this) return nullptr;
    __try {
        return hat::member_at<Simulation*>(this, OffsetProvider::MinecraftSim_mRenderSim);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

void MinecraftSim::setSimTimer(float timer)
{
    if (!this) return;
    auto game = getGameSim();
    auto render = getRenderSim();
    if (game && isMemoryWritable(&game->mTimer))
    {
        __try {
            game->mTimer = timer;
            game->mTimerMultiplier = timer / 20.f;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    if (render && isMemoryWritable(&render->mTimer))
    {
        __try {
            render->mTimer = timer;
            render->mTimerMultiplier = timer / 20.f;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
}

void MinecraftSim::setSimSpeed(float speed)
{
    if (!this) return;
    auto game = getGameSim();
    auto render = getRenderSim();
    if (game && isMemoryWritable(&game->mTimerMultiplier))
    {
        __try {
            game->mTimerMultiplier = speed;
            game->mTimer = 20.f * speed;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    if (render && isMemoryWritable(&render->mTimerMultiplier))
    {
        __try {
            render->mTimerMultiplier = speed;
            render->mTimer = 20.f * speed;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
}

float MinecraftSim::getSimTimer()
{
    if (!this) return 20.f;
    auto game = getGameSim();
    if (game && isMemoryReadable(&game->mTimer))
    {
        __try {
            return game->mTimer;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    return 20.f;
}

float MinecraftSim::getSimSpeed()
{
    if (!this) return 1.f;
    auto game = getGameSim();
    if (game && isMemoryReadable(&game->mTimerMultiplier))
    {
        __try {
            return game->mTimerMultiplier;
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    return 1.f;
}

GameSession* MinecraftSim::getGameSession()
{
    if (!this) return nullptr;
    __try {
        return hat::member_at<GameSession*>(this, OffsetProvider::MinecraftSim_mGameSession);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}
