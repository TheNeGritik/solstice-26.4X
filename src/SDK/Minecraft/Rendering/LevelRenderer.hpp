#pragma once
#include <Utils/MemUtils.hpp>
#include <Utils/Structs.hpp>


//
// Created by vastrakai on 7/7/2024.
//

class LevelRendererPlayer {
public:
    glm::vec3* getCameraPos();
    glmatrixf* getViewMatrix();
    float* getFovX();
    float* getFovY();
    void setFovX(float v);
    void setFovY(float v);
};

class LevelRenderer {
public:
    //CLASS_FIELD(LevelRendererPlayer*, mLevelRendererPlayer, 0x308);
    LevelRendererPlayer* getRendererPlayer();
};