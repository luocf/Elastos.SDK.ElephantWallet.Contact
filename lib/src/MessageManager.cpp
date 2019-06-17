//
//  MessageManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <MessageManager.hpp>

#include <ChannelImplCarrier.hpp>
#include <ChannelImplElaChain.hpp>
#include <Json.hpp>
#include <Log.hpp>
#include <Random.hpp>
#include <SafePtr.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
struct JsonKey {
    static constexpr const char* Did = "Did";
    static constexpr const char* Summary = "Summary";
    static constexpr const char* HumanInfo = "HumanInfo";

    static constexpr const char* MessageData = "MessageData";
};


/***********************************************/
/***** static function implement ***************/
/***********************************************/
NLOHMANN_JSON_SERIALIZE_ENUM(MessageManager::MessageType, {
    { MessageManager::MessageType::MsgText, "MsgText" },
    { MessageManager::MessageType::MsgAudio, "MsgAudio" },
    { MessageManager::MessageType::MsgTransfer, "MsgTransfer" },

    { MessageManager::MessageType::CtrlSyncDesc, "CtrlSyncDesc" },
    { MessageManager::MessageType::AckSyncDesc, "AckSyncDesc" },
});

inline void to_json(Json& j, const std::shared_ptr<MessageManager::MessageInfo>& info) {
    j = Json {
        {"Type", info->mType},
        {"PlainContent", info->mPlainContent},
        {"CryptoAlgorithm", info->mCryptoAlgorithm},
        {"TimeStamp", info->mTimeStamp},
    };
}

inline void from_json(const Json& j, std::shared_ptr<MessageManager::MessageInfo>& info) {
    info = MessageManager::MakeEmptyMessage();
    info->mType = j["Type"];
    info->mPlainContent = j["PlainContent"].get<std::vector<uint8_t>>();
    info->mCryptoAlgorithm = j["CryptoAlgorithm"];
    info->mTimeStamp = j["TimeStamp"];
}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::MakeEmptyMessage()
{
    struct Impl: MessageManager::MessageInfo {
    };

    auto msgInfo = std::make_shared<Impl>();

    return msgInfo;
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
MessageManager::MessageManager(std::weak_ptr<SecurityManager> sectyMgr,
                               std::weak_ptr<UserManager> userMgr,
                               std::weak_ptr<FriendManager> friendMgr)
    : mSecurityManager(sectyMgr)
    , mUserManager(userMgr)
    , mFriendManager(friendMgr)
    , mMessageListener()
    , mMessageChannelMap()
{
}

MessageManager::~MessageManager()
{
}

void MessageManager::setMessageListener(std::shared_ptr<MessageListener> listener)
{
    mMessageListener = listener;
    mMessageListener->mMessageManager = weak_from_this();
}

int MessageManager::presetChannels(std::weak_ptr<Config> config)
{
    bool hasFailed = false;

    std::map<ChannelType, std::string> profileMap;

    auto userMgr = SAFE_GET_PTR(mUserManager);
    std::shared_ptr<UserInfo> userInfo;
    userMgr->getUserInfo(userInfo);


    mMessageChannelMap[ChannelType::Carrier] = std::make_shared<ChannelImplCarrier>(static_cast<uint32_t>(ChannelType::Carrier),
                                                                                    mMessageListener,
                                                                                    config);
    // TODO mMessageChannelMap[ChannelType::ElaChain] = std::make_shared<ChannelImplElaChain>(config, mSecurityManager);

    for(const auto& channel: mMessageChannelMap) {
        int ret = channel.second->preset(""); // TODO add carrier secretkey
        if(ret < 0) {
            hasFailed = true;
            Log::W(Log::TAG, "Failed to preset channel %d", channel.first);
        }
    }
    if(hasFailed) {
        return ErrCode::ChannelFailedPresetAll;
    }

    return 0;
}

int MessageManager::openChannels()
{
    bool hasFailed = false;

    for(const auto& channel: mMessageChannelMap) {
        int ret = channel.second->open();
        if(ret < 0) {
            hasFailed = true;
            Log::W(Log::TAG, "Failed to open channel %d", channel.first);
        }
    }
    if(hasFailed) {
        return ErrCode::ChannelFailedOpenAll;
    }

    return 0;
}

int MessageManager::closehannels()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int MessageManager::requestFriend(const std::string& friendAddr,
                                  ChannelType humanChType,
                                  const std::string& summary,
                                  bool remoteRequest)
{
    auto it = mMessageChannelMap.find(humanChType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::ChannelNotFound;
    }
    auto channel = it->second;

    if(channel->isReady() == false) {
        return ErrCode::ChannelNotReady;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    std::shared_ptr<UserInfo> userInfo;
    userMgr->getUserInfo(userInfo);

    std::string userDid;
    int ret = userInfo->getHumanInfo(HumanInfo::Item::Did, userDid);
    if(ret < 0) {
        return ret;
    }

    std::string humanInfo;
    ret = userInfo->HumanInfo::serialize(humanInfo, true);
    if(ret < 0) {
        return ret;
    }

    Json jsonInfo = Json::object();
    jsonInfo[JsonKey::Did] = userDid;
    jsonInfo[JsonKey::Summary] = summary;
    jsonInfo[JsonKey::HumanInfo] = humanInfo;

    auto details = jsonInfo.dump();
    ret = channel->requestFriend(friendAddr, details, remoteRequest);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::makeMessage(MessageType type,
                                                                         const std::vector<uint8_t>& plainContent,
                                                                         const std::string& cryptoAlgorithm) const
{
    struct Impl: MessageManager::MessageInfo {
        Impl(MessageType type,
             const std::vector<uint8_t>& plainContent,
             const std::string& cryptoAlgorithm)
            : MessageManager::MessageInfo(type, plainContent, cryptoAlgorithm) {
        }
    };

    auto msgInfo = std::make_shared<Impl>(type, plainContent, cryptoAlgorithm);

    return msgInfo;
}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::makeTextMessage(const std::string& plainContent,
                                                                             const std::string& cryptoAlgorithm) const
{
    std::vector<uint8_t> plainContentBytes(plainContent.begin(), plainContent.end());
    return makeMessage(MessageType::MsgText, plainContentBytes, cryptoAlgorithm);
}

int MessageManager::sendMessage(const std::shared_ptr<HumanInfo> humanInfo,
                                ChannelType humanChType,
                                const std::shared_ptr<MessageInfo> msgInfo)
{
    auto it = mMessageChannelMap.find(humanChType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::ChannelNotFound;
    }
    auto channel = it->second;

    if(channel->isReady() == false) {
        return ErrCode::ChannelNotReady;
    }

    auto cryptoMsgInfo = makeMessage(msgInfo);
    if(msgInfo->mCryptoAlgorithm.empty() == true
    || msgInfo->mCryptoAlgorithm == "plain") {
        cryptoMsgInfo->mPlainContent = msgInfo->mPlainContent;
    } else {
        std::string pubKey;
        int ret = humanInfo->getHumanInfo(HumanInfo::Item::ChainPubKey, pubKey);
        if(ret < 0) {
            return ret;
        }

        auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
        ret = sectyMgr->encryptData(pubKey, msgInfo->mPlainContent, cryptoMsgInfo->mPlainContent);
        if(ret < 0) {
            return ret;
        }
    }

    Json jsonData = Json::object();
    jsonData[JsonKey::MessageData] = cryptoMsgInfo;
    //std::vector<uint8_t> data = Json::to_cbor(jsonData);
    std::string jsonStr = jsonData.dump();
    Log::W(Log::TAG, ">>>>>>>>>>>>> %s", jsonStr.c_str());
    std::vector<uint8_t> data(jsonStr.begin(), jsonStr.end());

    if(humanChType == ChannelType::Carrier) {
        std::vector<HumanInfo::CarrierInfo> infoArray;
        int ret = humanInfo->getAllCarrierInfo(infoArray);
        if(ret < 0) {
            return ErrCode::ChannelNotEstablished;
        }

        ret = ErrCode::ChannelNotOnline;
        for(auto& it: infoArray) {
            HumanInfo::Status status = HumanInfo::Status::Invalid;
            int r = humanInfo->getCarrierStatus(it.mUsrId, status);
            if(r < 0 || status != HumanInfo::Status::Online) {
                continue;
            }

            ret = channel->sendMessage(it.mUsrId, data);
            if(ret < 0) {
                return ret;
            }
        }
        return ret;
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    }
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
MessageManager::MessageInfo::MessageInfo(MessageType type,
                                         const std::vector<uint8_t>& plainContent,
                                         const std::string& cryptoAlgorithm)
    : mType(type)
    , mPlainContent(plainContent)
    , mCryptoAlgorithm(cryptoAlgorithm)
    , mTimeStamp(0)
{
    auto now = std::chrono::system_clock::now();
    mTimeStamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

MessageManager::MessageInfo::MessageInfo(const MessageInfo& info,
                                         bool ignoreContent)
    : mType(info.mType)
    , mPlainContent()
    , mCryptoAlgorithm(info.mCryptoAlgorithm)
    , mTimeStamp(info.mTimeStamp)
{
    if(ignoreContent == false) {
        mPlainContent = info.mPlainContent;
    }
}

MessageManager::MessageInfo::MessageInfo()
    : mType(MessageType::Empty)
    , mPlainContent()
    , mCryptoAlgorithm()
    , mTimeStamp(0)
{
}

MessageManager::MessageInfo::~MessageInfo()
{
}

void MessageManager::MessageListener::onStatusChanged(const std::string& userCode,
                                                      uint32_t channelType,
                                                      MessageManager::MessageListener::ChannelStatus status)
{
    int ret = ErrCode::UnknownError;

    auto msgMgr = SAFE_GET_PTR_NO_RETVAL(mMessageManager);
    auto userMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mUserManager);
    std::shared_ptr<UserInfo> userInfo;
    userMgr->getUserInfo(userInfo);

    ChannelType userChType = static_cast<ChannelType>(channelType);
    UserInfo::Status userStatus = (status == ChannelStatus::Online ? UserInfo::Status::Online : UserInfo::Status::Offline);

    UserInfo::Status oldStatus = userInfo->getHumanStatus();
    if(userChType == ChannelType::Carrier) {
        if(userStatus == UserInfo::Status::Online) {
            userMgr->setupMultiDevChannels();
        }
        ret = userInfo->setCarrierStatus(userCode, userStatus);
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    }
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to set status.");
        return;
    }

    UserInfo::Status newStatus = userInfo->getHumanStatus();
    if(newStatus != oldStatus) {
        onStatusChanged(userInfo, userChType, newStatus);
    }
}

void MessageManager::MessageListener::onReceivedMessage(const std::string& friendCode,
                                                        uint32_t channelType,
                                                        const std::vector<uint8_t>& msgContent)
{
    int ret = ErrCode::UnknownError;
    ChannelType humanChType = static_cast<ChannelType>(channelType);

    auto msgMgr = SAFE_GET_PTR_NO_RETVAL(mMessageManager);
    auto userMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mUserManager);
    auto friendMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mFriendManager);

    std::shared_ptr<UserInfo> userInfo;
    ret = userMgr->getUserInfo(userInfo);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to get user info.");
        return;
    }

    std::string jsonStr(msgContent.begin(), msgContent.end());
    Log::W(Log::TAG, "<<<<<<<<<<<<< %s", jsonStr.c_str());
    auto jsonData = Json::parse(jsonStr);
    //auto jsonData = Json::from_cbor(msgContent);
    std::shared_ptr<MessageInfo> cryptoMsgInfo = jsonData[JsonKey::MessageData];

    auto msgInfo = msgMgr->makeMessage(cryptoMsgInfo);
    if(cryptoMsgInfo->mCryptoAlgorithm.empty() == true
    || cryptoMsgInfo->mCryptoAlgorithm == "plain") {
        msgInfo->mPlainContent = cryptoMsgInfo->mPlainContent;
    } else {
        auto sectyMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mSecurityManager);
        ret = sectyMgr->decryptData(cryptoMsgInfo->mPlainContent, msgInfo->mPlainContent);
        if(ret < 0) {
            return;
        }
    }

    std::shared_ptr<HumanInfo> humanInfo;
    if(userMgr->contains(friendCode)) {
        humanInfo = userInfo;
    } else {
        std::shared_ptr<FriendInfo> friendInfo;
        ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
        if(ret < 0) {
            Log::E(Log::TAG, "Failed to get friend info.");
            return;
        }
        humanInfo = friendInfo;
    }

    if(msgInfo->mType == MessageType::CtrlSyncDesc
    || msgInfo->mType == MessageType::AckSyncDesc) {
        std::string humanDesc {msgInfo->mPlainContent.begin(), msgInfo->mPlainContent.end()};
        auto newInfo = HumanInfo();
        newInfo.HumanInfo::deserialize(humanDesc, true);
        if(ret < 0) {
            Log::E(Log::TAG, "Failed to process friend desc.");
            return;
        }

        humanInfo->mergeHumanInfo(newInfo, HumanInfo::Status::Online);

        if(msgInfo->mType == MessageType::CtrlSyncDesc) {
            // send latest user desc.
            std::string humanDesc;
            ret = userInfo->HumanInfo::serialize(humanDesc, true);
            if(ret < 0) {
                Log::E(Log::TAG, "Failed to serialize user human info.");
                return;
            }
            std::vector<uint8_t> humanDescBytes(humanDesc.begin(), humanDesc.end());
            auto msgInfo = msgMgr->makeMessage(MessageType::AckSyncDesc, humanDescBytes);
            ret = msgMgr->sendMessage(humanInfo, humanChType, msgInfo);
            if(ret < 0) {
                Log::E(Log::TAG, "Failed to send sync desc message.");
                return;
            }
        }
    }

    onReceivedMessage(humanInfo, humanChType, msgInfo);
}

void MessageManager::MessageListener::onSentMessage(int msgIndex, int errCode)
{
    Log::W(Log::TAG, "%s", __PRETTY_FUNCTION__);
}

void MessageManager::MessageListener::onFriendRequest(const std::string& friendCode,
                                                      uint32_t channelType,
                                                      const std::string& details)
{
    int ret = ErrCode::UnknownError;
    ChannelType humanChType = static_cast<ChannelType>(channelType);

    auto msgMgr = SAFE_GET_PTR_NO_RETVAL(mMessageManager);
    auto userMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mUserManager);
    auto friendMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mFriendManager);

    std::string friendDid;
    std::string summary;
    HumanInfo humanInfo;
    try {
        Json jsonInfo= Json::parse(details);
        friendDid = jsonInfo[JsonKey::Did];
        summary = jsonInfo[JsonKey::Summary];
        int ret = humanInfo.HumanInfo::deserialize(jsonInfo[JsonKey::HumanInfo], true);
        if(ret < 0) {
            friendDid = "";
            summary = details;
        }
    } catch(const std::exception& ex) {
        friendDid = "";
        summary = details;
    }
    if(humanChType == ChannelType::Carrier) {
        FriendInfo::CarrierInfo carrierInfo = {
            .mUsrAddr = "",
            .mUsrId = friendCode,
            .mDevInfo = {"", "", 0},
        };

        ret = humanInfo.addCarrierInfo(carrierInfo, HumanInfo::Status::WaitForAccept);
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    }
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to process friend code.");
        return;
    }

    std::shared_ptr<UserInfo> userInfo;
    ret = userMgr->getUserInfo(userInfo);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to process friend request, user info is not exists.");
        return;
    }

    std::string userDid;
    ret = userInfo->getHumanInfo(HumanInfo::Item::Did, userDid);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to process friend request, user did is not exists.");
        return;
    }

    if(friendDid == userDid) {
        ret = userInfo->mergeHumanInfo(humanInfo, HumanInfo::Status::Offline);
        if(ret < 0) {
            Log::E(Log::TAG, "Failed to add other dev.");
            return;
        }

        return;
    }

    std::shared_ptr<FriendInfo> friendInfo;
    auto friendStatus = HumanInfo::Status::Offline;
    ret = friendMgr->getFriendInfo(FriendInfo::HumanKind::Did, friendDid, friendInfo);
    if(ret < 0) { // not found
        friendInfo = std::make_shared<FriendInfo>(friendMgr);
        friendMgr->addFriendInfo(friendInfo);
        friendStatus = HumanInfo::Status::WaitForAccept;
    }
    ret = friendInfo->mergeHumanInfo(humanInfo, friendStatus);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to add other dev to friend.");
        return;
    }

    if(friendStatus == HumanInfo::Status::Offline) {
        ret = msgMgr->requestFriend(friendCode, humanChType, "", false);
    } else {
        msgMgr->mMessageListener->onFriendRequest(friendInfo, humanChType, summary);
    }

}

void MessageManager::MessageListener::onFriendStatusChanged(const std::string& friendCode,
                                                            uint32_t channelType,
                                                            MessageManager::MessageListener::ChannelStatus status)
{
    int ret = ErrCode::UnknownError;
    ChannelType humanChType = static_cast<ChannelType>(channelType);
    HumanInfo::Status humanStatus = (status == ChannelStatus::Online ? HumanInfo::Status::Online : HumanInfo::Status::Offline);

    auto msgMgr = SAFE_GET_PTR_NO_RETVAL(mMessageManager);
    auto userMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mUserManager);
    auto friendMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mFriendManager);

    std::shared_ptr<UserInfo> userInfo;
    ret = userMgr->getUserInfo(userInfo);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to process friend request, user info is not exists.");
        return;
    }

    std::string userDid;
    UserInfo::CarrierInfo info;
    ret = userInfo->getCarrierInfoByUsrId(friendCode, info);
    if(ret >= 0) { // found
        UserInfo::Status oldStatus = userInfo->getHumanStatus();
        if(humanChType == ChannelType::Carrier) {
            userInfo->setCarrierStatus(friendCode, humanStatus);
        } else {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
        }
        UserInfo::Status newStatus = userInfo->getHumanStatus();
        if(newStatus != oldStatus) {
            onStatusChanged(userInfo, humanChType, newStatus);
        }
    }

    std::shared_ptr<FriendInfo> friendInfo;
    ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
    if(ret >= 0) { // found
        FriendInfo::Status oldStatus = friendInfo->getHumanStatus();
        if(humanChType == ChannelType::Carrier) {
            friendInfo->setCarrierStatus(friendCode, humanStatus);
        } else {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
        }
        FriendInfo::Status newStatus = friendInfo->getHumanStatus();
        if(newStatus != oldStatus) {
            onFriendStatusChanged(friendInfo, humanChType, newStatus);
        }

        // send latest user desc.
        std::string humanDesc;
        ret = userInfo->HumanInfo::serialize(humanDesc, true);
        if(ret < 0) {
            Log::E(Log::TAG, "Failed to serialize user human info.");
            return;
        }
        std::vector<uint8_t> humanDescBytes(humanDesc.begin(), humanDesc.end());
        auto msgInfo = msgMgr->makeMessage(MessageType::CtrlSyncDesc, humanDescBytes);
        ret = msgMgr->sendMessage(friendInfo, humanChType, msgInfo);
        if(ret < 0) {
            Log::E(Log::TAG, "Failed to send sync desc message.");
            return;
        }
    }


    Log::D(Log::TAG, "onFriendStatusChanged() friendCode=%s, ret=%d", friendCode.c_str(), ret);
    return;
}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::makeMessage(std::shared_ptr<MessageManager::MessageInfo> from,
                                                                         bool ignoreContent)
{
    struct Impl: MessageManager::MessageInfo {
        Impl(const MessageInfo& info,
             bool ignoreContent)
            : MessageManager::MessageInfo(info, ignoreContent) {
        }
    };

    auto msgInfo = std::make_shared<Impl>(*from, ignoreContent);

    return msgInfo;
}

} // namespace elastos
