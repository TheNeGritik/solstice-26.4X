//
// Created by vastrakai on 7/25/2024.

#pragma once

#include <SDK/Minecraft/Actor/ActorPartModel.hpp>
#include "Event.hpp"

class BoneRenderEvent : public Event
{
public:
    std::unordered_map<int, ::std::vector<Bone>>* mBones;
    //class ActorPartModel* mPartModel;
    class Actor* mActor;

    // only used internally if mActor == localPlayer
    bool mDoBlockAnimation = false;

    explicit BoneRenderEvent(std::unordered_map<int, ::std::vector<Bone>>* bones, Actor* actor) : mBones(bones),  mActor(actor) {}
};