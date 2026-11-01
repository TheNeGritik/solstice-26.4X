#pragma once

#include "Event.hpp"

struct LevelRenderEvent : public Event {
    bool mUnkoMan;

    explicit LevelRenderEvent(bool UnkoMan) : mUnkoMan(UnkoMan) {}
};