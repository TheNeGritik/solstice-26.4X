//
// Created by vastrakai on 7/7/2024.
//

#include "LevelRenderer.hpp"

#include <libhat/Access.hpp>
#include <SDK/OffsetProvider.hpp>
#include <Utils/MemUtils.hpp>
#include <Windows.h>

glm::vec3* LevelRendererPlayer::getCameraPos()
{
    if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;
    __try {
        return &hat::member_at<glm::vec3>(this, OffsetProvider::LevelRendererPlayer_mCameraPos);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

glmatrixf* LevelRendererPlayer::getViewMatrix()
{
    if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;
    __try {
        return &hat::member_at<glmatrixf>(this, OffsetProvider::LevelRendererPlayer_mViewMatrix);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

float* LevelRendererPlayer::getFovX()
{
    if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;
    __try {
        return &hat::member_at<float>(this, OffsetProvider::LevelRendererPlayer_mFovX);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

float* LevelRendererPlayer::getFovY()
{
    if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;
    __try {
        return &hat::member_at<float>(this, OffsetProvider::LevelRendererPlayer_mFovY);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

void LevelRendererPlayer::setFovX(float v)
{
    if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return;
    __try {
        hat::member_at<float>(this, OffsetProvider::LevelRendererPlayer_mFovX) = v;
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

void LevelRendererPlayer::setFovY(float v)
{
    if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return;
    __try {
        hat::member_at<float>(this, OffsetProvider::LevelRendererPlayer_mFovY) = v;
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
}

LevelRendererPlayer* LevelRenderer::getRendererPlayer()
{
    if (!this || !MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(this))) return nullptr;
    __try {
        auto player = hat::member_at<LevelRendererPlayer*>(this, OffsetProvider::LevelRenderer_mRendererPlayer);
        if (player && MemUtils::isValidPtr(reinterpret_cast<uintptr_t>(player))) {
            return player;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return nullptr;
}
