#pragma once
#include "SubChunkStorage.hpp"

class SubChunk {
public:
    PAD(0x1c);
    bool                                     mHasMaxSkyLight;                // this+0x0020
    bool                                     mNeedsInitLighting;              // this+0x0021
    bool                                     mNeedsClientLighting;                    // this+0x0022
    std::unique_ptr< ::SubChunkStorage< ::Block> > mBlocks[2];                           // this+0x0028
    class SubChunkBlockStorage* mBlocksReadPtr[2];                     // this+0x0038
    PAD(0x18);                     // this+0x0040
    uint64_t mHash;
    bool mHashDirty;
    signed char mAbsoluteIndex;
    bool mIsReplacementSubChunk;
    unsigned char mRenderChunkTrackingVersionNumber;
    bool mIsInitialized;
};
