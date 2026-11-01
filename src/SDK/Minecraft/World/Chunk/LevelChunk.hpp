#pragma once
// all of this subchunk stuff was sent to me by tozic and
// im pretty sure its pasted from nuvola lmao
#include <SDK/OffsetProvider.hpp>

#include "SubChunk.hpp"

class LevelChunk {
public:
    PAD(0x78);
    int32_t                 mPositionX;               // this+0x0078
    int32_t                 mPositionY;               // this+0x007C
    PAD(0x70);
    std::atomic<unsigned char> mLoadState;
    PAD(0x37);
    int32_t                 mLastTick;               // this+0x0120
    PAD(0x10);
    std::vector<SubChunk>   subChunks;               // this+0x0138
    PAD(0xF84);
    int32_t                 mFinalized;
    PAD(0x401);
    bool                    isLoading;               // this+0x11BB
    PAD(0x3C8);

    std::vector<SubChunk> const& getSubChunks() {
        return hat::member_at<std::vector<SubChunk>>(this, OffsetProvider::LevelChunk_mSubChunks);
    }
};
