//
// Created by vastrakai on 6/30/2024.
//

#pragma once
#include <bitset>




struct MoveInputState
{
    enum class Flag : int {
        SneakDown = 0,
        SneakToggleDown = 1,
        WantDownSlow = 2,
        WantUpSlow = 3,
        BlockSelectDown = 4,
        AscendBlock = 5,
        DescendBlock = 6,
        JumpDown = 7,
        SprintDown = 8,
        UpLeft = 9,
        UpRight = 10,
        DownLeft = 11,
        DownRight = 12,
        Up = 13,
        Down = 14,
        Left = 15,
        Right = 16,
        Ascend = 17,
        Descend = 18,
        ChangeHeight = 19,
        LookCenter = 20,
        SneakInputCurrentlyDown = 21,
        SneakInputWasReleased = 22,
        SneakInputWasPressed = 23,
        JumpInputWasReleased = 24,
        JumpInputWasPressed = 25,
        JumpInputCurrentlyDown = 26,
        Count = 27,
    };

    std::bitset<27> FlagValues;
    glm::vec2 AnalogMoveVector;
    unsigned char LookSlightDirField;
    unsigned char LookNormalDirField;
    unsigned char LookSmoothDirField;
};
struct MoveInputComponent {
public:
    enum class Flag : int {
        Sneaking = 0,
        Sprinting = 1,
        WantUp = 2,
        WantDown = 3,
        Jumping = 4,
        AutoJumpingInWater = 5,
        MoveInputStateLocked = 6,
        PersistSneak = 7,
        AutoJumpEnabled = 8,
        IsCameraRelativeMovementEnabled = 9,
        IsRotControlledByMoveDirection = 10,
        Count = 11,
    };

    MoveInputState mInputState;
    MoveInputState mRawInputState;
    unsigned char mHoldAutoJumpInWaterTicks;
    glm::vec2 mMove;
    glm::vec2 mLookDelta;
    glm::vec2 mInteractDir;
    glm::vec3 mDisplacement;
    glm::vec3 mDisplacementDelta;
    glm::vec3 mCameraOrientation;
    std::bitset<8> mFlagValues;
    std::array<bool, 2> mIsPaddling;

    bool isUp() {
        return mInputState.FlagValues.test((size_t)MoveInputState::Flag::Up);
    }
    void setUp(bool val) {
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::Up, val);
    }

    bool isDown() {
        return mInputState.FlagValues.test((size_t)MoveInputState::Flag::Down);
    }
    void setDown(bool val) {
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::Down, val);
    }

    bool isLeft() {
        return mInputState.FlagValues.test((size_t)MoveInputState::Flag::Left);
    }
    void setLeft(bool val) {
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::Left, val);
    }

    bool isRight() {
        return mInputState.FlagValues.test((size_t)MoveInputState::Flag::Right);
    }
    void setRight(bool val) {
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::Right, val);
    }

    bool isJumping() {
        return mInputState.FlagValues.test((size_t)MoveInputState::Flag::JumpDown);
    }
    void setJumping(bool val) {
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::JumpDown, val);
        mRawInputState.FlagValues.set((size_t)MoveInputState::Flag::JumpDown, val);
    }

    bool isSneaking() {
        return mInputState.FlagValues.test((size_t)MoveInputState::Flag::SneakDown);
    }
    void setSneaking(bool val) {
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::SneakDown, val);
        mRawInputState.FlagValues.set((size_t)MoveInputState::Flag::SneakDown, val);
    }

    bool isSprinting() {
        return mInputState.FlagValues.test((size_t)MoveInputState::Flag::SprintDown);
    }
    void setSprinting(bool val) {
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::SprintDown, val);
        mRawInputState.FlagValues.set((size_t)MoveInputState::Flag::SprintDown, val);
    }

    void reset(bool lockMove = false, bool resetMove = true) { // tabun attenai :retard:
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::SneakDown, false);
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::JumpDown, false);
        mInputState.FlagValues.set((size_t)MoveInputState::Flag::SprintDown, false);
        if (resetMove)
        {
            mInputState.FlagValues.set((size_t)MoveInputState::Flag::Up, false);
            mInputState.FlagValues.set((size_t)MoveInputState::Flag::Down, false);
            mInputState.FlagValues.set((size_t)MoveInputState::Flag::Left, false);
            mInputState.FlagValues.set((size_t)MoveInputState::Flag::Right, false);
        }
        mMove = glm::vec2(0, 0);
    }
};

/*
struct MoveInputComponent {
public:
    CLASS_FIELD(bool, mIsMoveLocked, 0x8A);
    CLASS_FIELD(bool, mIsSneakDown, 0x28);
    CLASS_FIELD(bool, mIsJumping, 0x2F);
    CLASS_FIELD(bool, mIsJumping2, 0x80);
    CLASS_FIELD(bool, mIsSprinting, 0x30);
    CLASS_FIELD(bool, mForward, 0xD);
    CLASS_FIELD(bool, mBackward, 0xE);
    CLASS_FIELD(bool, mLeft, 0xF);
    CLASS_FIELD(bool, mRight, 0x10);
    CLASS_FIELD(glm::vec2, mMoveVector, 0x48);

    // padding to make the struct size 136
    PAD(0x88);

    void setJumping(bool value) {
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x26)[0] = value;
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x80)[0] = value;
    }

    void reset(bool lockMove = false, bool resetMove = true) {
        mIsMoveLocked = lockMove;
        mIsSneakDown = false;
        mIsJumping = false;
        mIsJumping2 = false;
        mIsSprinting = false;
        if (resetMove)
        {
            mForward = false;
            mBackward = false;
            mLeft = false;
            mRight = false;
        }
        mMoveVector = glm::vec2(0, 0);
    }
};

*/

struct RawMoveInputComponent { // mendoi
public:
    glm::vec2 mRawMove;

    void setJumping(bool value) {
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x26)[0] = value;
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x80)[0] = value;
    }
};


/*
struct RawMoveInputComponent {
public:
    CLASS_FIELD(bool, mIsMoveLocked, 0x82);
    CLASS_FIELD(bool, mIsSneakDown, 0x20);
    CLASS_FIELD(bool, mIsJumping, 0x26);
    CLASS_FIELD(bool, mIsJumping2, 0x80);
    CLASS_FIELD(bool, mIsSprinting, 0x27);
    CLASS_FIELD(bool, mForward, 0x2C);
    CLASS_FIELD(bool, mBackward, 0x2D);
    CLASS_FIELD(bool, mLeft, 0x2E);
    CLASS_FIELD(bool, mRight, 0x2F);
    CLASS_FIELD(glm::vec2, mMoveVector, 0x48);

    // padding to make the struct size 136
    char pad_0x0[0x88];

    void setJumping(bool value) {
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x26)[0] = value;
        reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x80)[0] = value;
    }
};
*/

//static_assert(sizeof(MoveInputComponent) == 136, "MoveInputComponent size is not 136 bytes!");
//static_assert(sizeof(RawMoveInputComponent) == 136, "RawMoveInputComponent size is not 136 bytes!");