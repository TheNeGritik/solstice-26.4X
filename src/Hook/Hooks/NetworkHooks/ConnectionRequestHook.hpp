#pragma once
//
// Created by vastrakai on 7/3/2024.
//

#include <Hook/Hook.hpp>


/*
class ConnectionRequest* result, class PrivateKeyManager* privKeyManager,
                          class Certificate* cert, std::string* selfSignedId, std::string* serverAddress, int64_t clientRandomId,
                          std::string* skinId, const char* skinData, const char* capeData, class SerializedSkin* skin,
                          std::string* deviceId, int inputMode, int uiProfile, int guiScale, std::string* languageCode,
                          bool isEditorMode, bool IsEduMode, std::string* tenantId, int8_t adRole, std::string* platformUserId,
                          std::string* thirdPartyName, bool thirdPartyNameOnly, std::string* platformOnlineID, std::string* platformOfflineID,
                          std::string* capeId, bool CompatibleWithClientSideChunkGen*/
                          
class ConnectionRequestHook : public Hook {
public:
    ConnectionRequestHook() : Hook()
    {
        mName = "ConnectionRequest::create";
    };

    static std::unique_ptr<Detour> mDetour;

    static void* createRequestDetourFunc(
        __int64 _this,
        __int64/*Bedrock::NonOwnerPointer<::AppPlatform>*/ appPlatform,
        class ConnectionAuthInfo* authInfo,
        std::string* selfSignedId,
        std::string* serverAddress,
        unsigned __int64 clientRandomId,
        class ConnectionSkinInfo const* skinInfo,
        std::string* deviceId,
        int /*InputMode*/ currentInputMode,
        int guiScale,
        std::string const& languageCode,
        bool clientIsEditorCapable,
        int /*EditorConnectionJoinIntent*/ clientEditorConnectionIntent,
        bool isEduMode,
        void* /*std::optional<class WebToken>*/ eduTokenChain,
        std::string eduSessionToken,
        std::string eduJoinerToHostNonce,
        unsigned char /*edu::Role*/  classRole,
        std::string const& platformId,
        std::string const& thirdPartyName,
        std::string const& platformOnlineId,
        std::string const& platformOfflineId,
        std::optional<class PlayerPartyInfo> const& partyInfo,
        bool compatibleWithClientSideChunkGen,
        struct SyncedClientOptionsComponent const& options,
        void* /*std::optional<class Nonce> const&*/  nonce
    );
    void init() override;
};

