//
// Created by vastrakai on 7/12/2024.
//

#pragma once
#include <SDK/SigManager.hpp>
#include <string>
#include <unordered_map>
#include <vector>

enum AttributeId {
    Health = 5,
    Absorption = 12,
};

class AttributeInstance {
    class Attribute* mAttribute;
    PAD(0x58);

    union {
        float mDefaultValues[3];
        struct {
            float mDefaultMinValue;
            float mDefaultMaxValue;
            float mDefaultValue;
        };
    };

    union {
        float mCurrentValues[3];
        struct {
            float mCurrentMinValue;
            float mCurrentMaxValue;
            float mCurrentValue;
        };
    };
    bool mValueChangedDuringTick;

    virtual ~AttributeInstance();
    virtual void tick();
};

static_assert(sizeof(AttributeInstance) == 0x88, "AttributeInstance size is not correct");

class Attribute {
public: // I'm pretty sure this is a HashedString but i don't really care
    __int64 mHash;
    __int64 mHashedStringHash;
    std::string mAttributeName;
    PAD(0x32);

    Attribute() {
        memset(this, 0x0, sizeof(Attribute));
    }

    Attribute(__int64 hash) {
        memset(this, 0x0, sizeof(Attribute));
        this->mHash = hash;
    }
};

enum AttributeHashes : unsigned __int64 {
    HEALTH = 30064771328 - 4294967296,
    HUNGER = 8589934848 - 4294967296,
    MOVEMENT = 42949673217 - 4294967296,
    ABSORPTION = 60129542401 - 4294967296,
    SATURATION = 12884902144 - 4294967296,
    FOLLOW_RANGE = 34359738369 - 4294967296,
    LEVEL = 21474836736 - 4294967296,
    EXPERIENCE = 25769804032 - 4294967296
};

class HealthAttribute : public Attribute
{
public:
    HealthAttribute() { this->mHash = AttributeHashes::HEALTH; }
};

class PlayerHungerAttribute : public Attribute
{
public:
    PlayerHungerAttribute() { this->mHash = AttributeHashes::HUNGER; }
};

class MovementAttribute : public Attribute
{
public:
    MovementAttribute() { this->mHash = AttributeHashes::MOVEMENT; }
};

class AbsorptionAttribute : public Attribute
{
public:
    AbsorptionAttribute() { this->mHash = AttributeHashes::ABSORPTION; }
};

class PlayerSaturationAttribute : public Attribute
{
public:
    PlayerSaturationAttribute() { this->mHash = AttributeHashes::SATURATION; }
};

class FollowRangeAttribute : public Attribute
{
public:
    FollowRangeAttribute() { this->mHash = AttributeHashes::FOLLOW_RANGE; }
};

class PlayerLevelAttribute : public Attribute {
public:
    PlayerLevelAttribute() { this->mHash = AttributeHashes::LEVEL; }
};

class PlayerExperienceAttribute : public Attribute
{
public:
    PlayerExperienceAttribute() { this->mHash = AttributeHashes::EXPERIENCE; }
};

class BaseAttributeMap 
{
public:
    std::vector<uint32_t> mids;
    std::vector<AttributeInstance> minstances;
private:
    char pad[0x20];
public:

    AttributeInstance* getInstance(uint32_t id)
    {
        return &minstances[id];
    }
};

static_assert(sizeof(BaseAttributeMap) == 0x50); 


struct AttributesComponent
{
    BaseAttributeMap mBaseAttributeMap;
};

static_assert(sizeof(AttributesComponent) == 0x50); 