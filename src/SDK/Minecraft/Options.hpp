#pragma once  
//  
// Created by vastrakai on 7/8/2024.  
//  
#include <xmmintrin.h>
#include <vector>  
#include <Utils/MemUtils.hpp>  
class Option {
public:
    class Impl {
    public:
        PAD(0x2a0);

    };
    std::unique_ptr<Option::Impl> mImpl;

    virtual ~Option(); // need for size SHIT CLANG
};
class IntOption : public Option { 
public:  
    int32_t mMaxValue;
    int32_t mMinValue;
    int32_t mValue;
    int32_t mDefaultValue;
    bool mClampToRange;
    PAD(0x58);
};  




class FloatOption : public Option{  
public:  
    float mMaxValue;
    float mMinValue;
    float mValue;
    float mDefaultValue;
    float mDelta;
};  

static_assert(sizeof(IntOption) == 0x80);
static_assert(sizeof(FloatOption) == 0x28);


class Options {  
public:  
    void* vtable;
    std::array<std::unique_ptr<class Option>, 813> options;/*TODO move to array*/

    CLASS_FIELD(IntOption*, mThirdPerson, 0x28);  
    CLASS_FIELD(FloatOption*, mGfxFieldOfView, 0x188); // 1.26.45: 0x188 is FOV
    CLASS_FIELD(FloatOption*, mGfxGamma, 0x1A0);       // 1.26.45: 0x1A0 is Gamma

    FloatOption* getGammaOption() {
        return mGfxGamma;
    }

    FloatOption* getFovOption() {
        return mGfxFieldOfView;
    }
};
