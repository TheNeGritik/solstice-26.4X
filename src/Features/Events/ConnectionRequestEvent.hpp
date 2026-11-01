//
// Created by vastrakai on 7/3/2024.
//

#pragma once

#include "Event.hpp"

/*
PrivateKeyManager* privKeyManager,
    Certificate* cert, std::string* selfSignedId, std::string* serverAddress, int64_t clientRandomId,
    std::string* skinId, const char* skinData, const char* capeData, SerializedSkin* skin, std::string* deviceId,
    int inputMode, int uiProfile, int guiScale, std::string* languageCode, bool isEditorMode, bool IsEduMode,
    std::string* tenantId, int8_t adRole, std::string* platformUserId, std::string* thirdPartyName,
    bool thirdPartyNameOnly, std::string* platformOnlineID, std::string* platformOfflineID, std::string* capeId,
    bool compatibleWithClientSideChunkGen*/
    
class ConnectionRequestEvent : public Event
{
public: //why need many
    //class ConnectionRequest* mResult;
    //class PrivateKeyManager* mPrivKeyManager;
    //class Certificate* mCert;
    std::string mSelfSignedId;
    std::string mServerAddress;
    int64_t mClientRandomId;
    std::string mSkinId;
    //const char* mSkinData;
    //const char* mCapeData;
    //class SerializedSkin* mSkin;
    std::string mDeviceId;
    int8_t mInputMode;
    //int mUiProfile;
    //int mGuiScale;
    //std::string* mLanguageCode;
    //bool mIsEditorMode;
    //bool mIsEduMode;
    //std::string* mTenantId;
    //int8_t mAdRole;
    //std::string* mPlatformUserId;
    //std::string* mThirdPartyName;
    //bool mThirdPartyNameOnly;
    //std::string* mPlatformOnlineID;
    //std::string* mPlatformOfflineID;
    //std::string* mCapeId;
    //bool mCompatibleWithClientSideChunkGen;
    
    explicit ConnectionRequestEvent(std::string selfsing, std::string serveraddr,__int64 clientid, std::string skinid, std::string devid, int8_t inputmode) :
            mSelfSignedId(selfsing),mServerAddress(serveraddr),mClientRandomId(clientid),mSkinId(skinid),mDeviceId(devid),mInputMode(inputmode){};
};



