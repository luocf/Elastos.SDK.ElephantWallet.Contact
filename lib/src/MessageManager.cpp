//
//  MessageManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <MessageManager.hpp>

#include <ChannelImplCarrier.hpp>
#include <ChannelImplElaChain.hpp>
#include <EnumMask.hpp>
#include <JsonDefine.hpp>
#include <Log.hpp>
#include <Random.hpp>
#include <SafePtr.hpp>
//#include "BlkChnClient.hpp"

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/
std::shared_ptr<MessageManager::MessageInfo> MessageManager::MakeEmptyMessage()
{
    struct Impl: MessageManager::MessageInfo {
    };

    auto msgInfo = std::make_shared<Impl>();

    return msgInfo;
}

std::shared_ptr<MessageManager::FileInfo> MessageManager::MakeEmptyFileInfo()
{
    struct Impl: MessageManager::FileInfo {
    };

    auto fileInfo = std::make_shared<Impl>();

    return fileInfo;
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

void MessageManager::setDataListener(std::shared_ptr<DataListener> listener)
{
    mDataListener = listener;
    mDataListener->mMessageManager = weak_from_this();
}

int MessageManager::presetChannels(std::weak_ptr<Config> config)
{
    bool hasFailed = false;

    std::map<ChannelType, std::string> profileMap;

    auto userMgr = SAFE_GET_PTR(mUserManager);
    std::shared_ptr<UserInfo> userInfo;
    userMgr->getUserInfo(userInfo);

    // CarrierInfo carrierInfo = {"", "", {"", "", 0}};
    // int ret = userInfo->getCurrDevCarrierInfo(carrierInfo);
    // if(ret < 0) {
    //     Log::W(Log::TAG, "Failed to get current dev carrier info.");
    // }

    mMessageChannelMap[ChannelType::Carrier] = std::make_shared<ChannelImplCarrier>(static_cast<uint32_t>(ChannelType::Carrier),
                                                                                    mMessageListener, mDataListener,
                                                                                    config);
    // TODO mMessageChannelMap[ChannelType::ElaChain] = std::make_shared<ChannelImplElaChain>(config, mSecurityManager);

    for(const auto& channel: mMessageChannelMap) {
        std::string profile;
        // if(channel.first == ChannelType::Carrier) {
        //     std::string carrierSecKey;
        //     int ret = userInfo->getIdentifyCode(UserInfo::Type::CarrierSecKey, carrierSecKey);
        //     if (ret == 0) {
        //         profile = carrierSecKey;
        //     } else {
        //         Log::W(Log::TAG, "Failed to get current dev carrier secret key.");
        //     }
        // }

        int ret = channel.second->preset(profile); // TODO add carrier secretkey
        if(ret < 0) {
            hasFailed = true;
            Log::W(Log::TAG, "Failed to preset channel %d", channel.first);
        }

        // if(channel.first == ChannelType::Carrier) {
        //     std::string validSecKey;
        //     ret = channel.second->getSecretKey(validSecKey);
        //     if(ret < 0) {
        //         hasFailed = true;
        //         Log::W(Log::TAG, "Failed to get carrier secret key.");
        //     }

        //     std::string carrierSecKey;
        //     int ret = userInfo->getIdentifyCode(UserInfo::Type::CarrierSecKey, carrierSecKey);
        //     if (ret == 0) {
        //         if(validSecKey != carrierSecKey) {
        //             hasFailed = true;
        //             Log::W(Log::TAG, "Failed to check current dev carrier secret key.");
        //         }
        //     }
        // }
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

int MessageManager::closeChannels()
{
    bool hasFailed = false;

    for(const auto& channel: mMessageChannelMap) {
        int ret = channel.second->close();
        if(ret < 0) {
            hasFailed = true;
            Log::W(Log::TAG, "Failed to close channel %d", channel.first);
        }
    }
    if(hasFailed) {
        return ErrCode::ChannelFailedCloseAll;
    }

    return 0;
}

int MessageManager::requestFriend(const std::string& friendAddr,
                                  ChannelType humanChType,
                                  const std::string& summary,
                                  bool remoteRequest,
                                  bool forceRequest)
{
    Log::W(Log::TAG, ">>>>>>>>>>>>> requestFriend code:%s, details=%s", friendAddr.c_str(), summary.c_str());
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
    CHECK_ERROR(ret)

    std::string humanInfo;
    ret = userInfo->HumanInfo::serialize(humanInfo, true);
    CHECK_ERROR(ret)

    Json jsonInfo = Json::object();
    jsonInfo[JsonKey::Did] = userDid;
    jsonInfo[JsonKey::Summary] = summary;
    //jsonInfo[JsonKey::HumanInfo] = humanInfo;

    auto details = jsonInfo.dump();
    ret = channel->requestFriend(friendAddr, details, remoteRequest, forceRequest);
    if(ret == ErrCode::ChannelFailedFriendSelf
    || ret == ErrCode::ChannelFailedFriendExists) { // ignore to set error
        return ret;
    }
    CHECK_ERROR(ret)

    return 0;
}

int MessageManager::updateFriend(const std::string& did)
{
    auto userMgr = SAFE_GET_PTR(mUserManager);
    auto friendMgr = SAFE_GET_PTR(mFriendManager);

    std::shared_ptr<HumanInfo> humanInfo;
    if (userMgr->contains(did)) {
        std::shared_ptr<elastos::UserInfo> userInfo;
        std::ignore = userMgr->getUserInfo(userInfo);
        humanInfo = userInfo;
    } else if (friendMgr->contains(did)) {
        std::shared_ptr<FriendInfo> friendInfo;
        std::ignore  = friendMgr->tryGetFriendInfo(did, friendInfo);
        humanInfo = friendInfo;
    } else {
        Log::E(Log::TAG, "MessageManager::updateFriend() Failed for did: %s", did.c_str());
        return ErrCode::InvalidArgument;
    }

    std::vector<HumanInfo::CarrierInfo> carrierInfoArray;
    int ret = humanInfo->getAllCarrierInfo(carrierInfoArray);
    CHECK_ERROR(ret)

    int lastRet = 0;
    Log::I(Log::TAG, "MessageManager::updateFriend() =================== %d",
           carrierInfoArray.size());
    for(const auto& carrierInfo: carrierInfoArray) {
    Log::I(Log::TAG, "MessageManager::updateFriend() ===================");
        bool forceRequest = false;
        HumanInfo::Status status;
        std::ignore = humanInfo->getCarrierStatus(carrierInfo.mUsrId, status);
        if(status == HumanInfo::Status::WaitForAccept) {
            forceRequest = true;
        }

        int ret = requestFriend(carrierInfo.mUsrAddr,
                                MessageManager::ChannelType::Carrier,
                                "", true, forceRequest);
        Log::I(Log::TAG, "MessageManager::updateFriend() add carrier address %s", carrierInfo.mUsrAddr.c_str());
        if(ret < 0
           && ret != ErrCode::ChannelFailedFriendSelf
           && ret != ErrCode::ChannelFailedFriendExists) {
            Log::W(Log::TAG, "MessageManager::updateFriend() Failed to add %s ret=%d", carrierInfo.mUsrAddr.c_str(), ret);
            lastRet = ret;
        }
    }

    return lastRet;
}

int MessageManager::removeFriend(const std::string& friendCode, ChannelType humanChType)
{
    Log::W(Log::TAG, ">>>>>>>>>>>>> removeFriend code:%s", friendCode.c_str());
    auto it = mMessageChannelMap.find(humanChType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::ChannelNotFound;
    }
    auto channel = it->second;

    if(channel->isReady() == false) {
        return ErrCode::ChannelNotReady;
    }

    int ret = channel->removeFriend(friendCode);
    CHECK_ERROR(ret)

    return 0;
}

//int MessageManager::monitorDidChainCarrierID(const std::string& did)
//{
//    auto callback = [=](int errcode,
//                        const std::string& keyPath,
//                        const std::string& result) {
//        Log::D(Log::TAG, "MessageManager::monitorDidChainCarrierID() ecode=%d, path=%s, result=%s", errcode, keyPath.c_str(), result.c_str());
//        CHECK_RETVAL(errcode);
//
//        auto userMgr = SAFE_GET_PTR_NO_RETVAL(mUserManager);
//        auto friendMgr = SAFE_GET_PTR_NO_RETVAL(mFriendManager);
//
//        std::shared_ptr<HumanInfo> humanInfo;
//        if (userMgr->contains(did)) {
//            std::shared_ptr<elastos::UserInfo> userInfo;
//            std::ignore = userMgr->getUserInfo(userInfo);
//            humanInfo = userInfo;
//        } else if (friendMgr->contains(did)) {
//            std::shared_ptr<FriendInfo> friendInfo;
//            std::ignore  = friendMgr->tryGetFriendInfo(did, friendInfo);
//            humanInfo = friendInfo;
//        } else {
//            Log::E(Log::TAG, "MessageManager::monitorDidChainCarrierID() Failed to process CarrierId for did: %s", did.c_str());
//            CHECK_RETVAL(ErrCode::InvalidFriendCode);
//            return;
//        }
//
//        std::vector<std::string> values;
//        Json jsonPropArray = Json::parse(result);
//        for (const auto& it : jsonPropArray) {
//            values.push_back(it["value"]);
//        }
//
//        bool carrierInfoChanged = false;
//        for(const auto& it: values) {
//            HumanInfo::CarrierInfo carrierInfo;
//            int ret = HumanInfo::deserialize(it, carrierInfo);
//            if(ret < 0) {
//                Log::W(Log::TAG, "MessageManager::monitorDidChainCarrierID() Failed to sync CarrierId: %s", it.c_str());
//                continue; // ignore error
//            }
//
//            ret = humanInfo->addCarrierInfo(carrierInfo, HumanInfo::Status::WaitForAccept);
//            if(ret < 0) {
//                if(ret == ErrCode::IgnoreMergeOldInfo) {
//                    Log::V(Log::TAG, "MessageManager::monitorDidChainCarrierID() Ignore to sync CarrierId: %s", it.c_str());
//                } else {
//                    Log::E(Log::TAG, "MessageManager::monitorDidChainCarrierID() Failed to sync carrier info. CarrierId: %s", it.c_str());
//                }
//                continue; // ignore error
//            } else {
//                carrierInfoChanged = true;
//            }
//        }
//
////        if(carrierInfoChanged == false) {
////            Log::D(Log::TAG, "MessageManager::monitorDidChainCarrierID() did %s, CarrierId not changed.", did.c_str());
////            return;
////        }
////
//        int ret = updateFriend(did);
//        if(ret < 0) {
//            Log::E(Log::TAG, "MessageManager::monitorDidChainCarrierID() Failed to update friend. ret=%d", ret);
//            return;
//        }
//    };
//
//    auto bcClient = BlkChnClient::GetInstance();
//
//    std::string keyPath;
//    int ret = bcClient->getDidPropHistoryPath(did, "CarrierID", keyPath);
//    if (ret < 0) {
//        return ret;
//    }
//
//    Log::I(Log::TAG, "MessageManager::monitorDidChainCarrierID() keyPath=%s", keyPath.c_str());
//    ret = bcClient->appendMoniter(keyPath, callback);
//    if (ret < 0) {
//        return ret;
//    }
//
//    return 0;
//}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::MakeMessage(MessageType type,
                                                                         const std::vector<uint8_t>& plainContent,
                                                                         const std::string& cryptoAlgorithm)
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

std::shared_ptr<MessageManager::MessageInfo> MessageManager::MakeTextMessage(const std::string& plainContent,
                                                                             const std::string& cryptoAlgorithm)
{
    std::vector<uint8_t> plainContentBytes(plainContent.begin(), plainContent.end());
    return MakeMessage(MessageType::MsgText, plainContentBytes, cryptoAlgorithm);
}

std::shared_ptr<MessageManager::FileInfo> MessageManager::MakeFileInfo(const std::string& devId,
                                                                       const std::string& name,
                                                                       uint64_t size,
                                                                       const std::string& md5)
{
    struct Impl: MessageManager::FileInfo {
        Impl(const std::string& devId,
             const std::string& name,
             uint64_t size,
             const std::string& md5)
                : MessageManager::FileInfo(devId, name, size, md5) {
        }
    };

    auto fileInfo = std::make_shared<Impl>(devId, name, size, md5);

    return fileInfo;
}

int MessageManager::sendMessage(const std::shared_ptr<HumanInfo> humanInfo,
                                ChannelType humanChType,
                                const std::shared_ptr<MessageInfo> msgInfo,
                                bool sendToOtherDev)
{
    auto it = mMessageChannelMap.find(humanChType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::ChannelNotFound;
    }
    auto channel = it->second;

    if(channel->isReady() == false) {
        return ErrCode::ChannelNotReady;
    }

    //if(userMgr->contains(humanInfo)) {
        //return ErrCode::InvalidArgument;
    //}

    auto cryptoMsgInfo = MakeMessage(msgInfo);
    if(msgInfo->mCryptoAlgorithm.empty() == true
    || msgInfo->mCryptoAlgorithm == "plain") {
        cryptoMsgInfo->mPlainContent = msgInfo->mPlainContent;
    } else {
        std::string pubKey;
        int ret = humanInfo->getHumanInfo(HumanInfo::Item::ChainPubKey, pubKey);
        CHECK_ERROR(ret)

        auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
        ret = sectyMgr->encryptData(pubKey, msgInfo->mPlainContent, cryptoMsgInfo->mPlainContent);
        CHECK_ERROR(ret)
    }

    Json jsonData = Json::object();
    jsonData[JsonKey::MessageData] = cryptoMsgInfo;
    //std::vector<uint8_t> data = Json::to_cbor(jsonData);
    std::string jsonStr = jsonData.dump();
    Log::W(Log::TAG, ">>>>>>>>>>>>> %s", jsonStr.c_str());
    std::vector<uint8_t> data(jsonStr.begin(), jsonStr.end());

    if(humanChType == ChannelType::Carrier) {
        std::vector<HumanInfo::CarrierInfo> infoArray;

        auto userMgr = SAFE_GET_PTR(mUserManager);
        std::shared_ptr<UserInfo> userInfo;
        userMgr->getUserInfo(userInfo);

        if(sendToOtherDev == true) {
            int ret = userInfo->getAllCarrierInfo(infoArray);
            if (ret < 0) {
                return ErrCode::ChannelNotEstablished;
            }
        }

        std::vector<HumanInfo::CarrierInfo> friendArray;
        int ret = humanInfo->getAllCarrierInfo(friendArray);
        if(ret < 0) {
            return ErrCode::ChannelNotEstablished;
        }
//        for(auto& it: infoArray) {
//            Log::I(Log::TAG, "+++++++++++ %s", it.mUsrId.c_str());
//        }
        infoArray.insert(infoArray.end(), friendArray.begin(), friendArray.end());
        for(auto& it: infoArray) {
            Log::I(Log::TAG, "----------- %s", it.mUsrId.c_str());
        }

        ret = ErrCode::ChannelNotOnline;
        for(auto& it: infoArray) {
            HumanInfo::Status status1 = HumanInfo::Status::Invalid;
            HumanInfo::Status status2 = HumanInfo::Status::Invalid;
            userInfo->getCarrierStatus(it.mUsrId, status1);
            humanInfo->getCarrierStatus(it.mUsrId, status2);
            if(status1 != HumanInfo::Status::Online
            && status2 != HumanInfo::Status::Online) {
                continue;
            }

            Log::I(Log::TAG, ">>>>>>>>>>> send message to %s", it.mUsrId.c_str());
            ret = channel->sendMessage(it.mUsrId, data);
            if(ret < 0) {
                //return;
            }
        }
        return ret;
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    }
}

int MessageManager::pullData(const std::shared_ptr<HumanInfo> humanInfo,
                             ChannelType humanChType,
                             const std::string& devId,
                             const std::string& dataId)
{
    auto it = mMessageChannelMap.find(humanChType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::ChannelNotFound;
    }
    auto channel = it->second;

    if(channel->isReady() == false) {
        return ErrCode::ChannelNotReady;
    }

    if(humanChType == ChannelType::Carrier) {
        HumanInfo::CarrierInfo carrierInfo;

        int ret = humanInfo->getCarrierInfoByDevId(devId, carrierInfo);
        CHECK_ERROR(ret);

        std::vector<uint8_t> dataIdBytes(dataId.begin(), dataId.end());
        auto msgInfo = MakeMessage(MessageType::CtrlPullData, dataIdBytes);

        Json jsonData = Json::object();
        jsonData[JsonKey::MessageData] = msgInfo;
        std::string jsonStr = jsonData.dump();
        std::vector<uint8_t> data(jsonStr.begin(), jsonStr.end());

        ret = channel->sendMessage(carrierInfo.mUsrId, data);
        CHECK_ERROR(ret);

        return 0;
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    }
}

int MessageManager::cancelPullData(const std::shared_ptr<HumanInfo> humanInfo,
                                   ChannelType humanChType,
                                   const std::string& devId,
                                   const std::string& dataId)
{
    auto it = mMessageChannelMap.find(humanChType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::ChannelNotFound;
    }
    auto channel = it->second;

    if(channel->isReady() == false) {
        return ErrCode::ChannelNotReady;
    }

    if(humanChType == ChannelType::Carrier) {
        HumanInfo::CarrierInfo carrierInfo;

        int ret = humanInfo->getCarrierInfoByDevId(devId, carrierInfo);
        CHECK_ERROR(ret);

        ret = channel->cancelSendData(carrierInfo.mUsrId, dataId);
        CHECK_ERROR(ret);

        return 0;
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    }
}


int MessageManager::broadcastDesc(ChannelType chType)
{
    auto userMgr = SAFE_GET_PTR(mUserManager);
    auto friendMgr = SAFE_GET_PTR(mFriendManager);

    std::vector<std::shared_ptr<HumanInfo>> broadcastList;

    std::shared_ptr<UserInfo> userInfo;
    int ret = userMgr->getUserInfo(userInfo);
    CHECK_ERROR(ret)
    broadcastList.push_back(userInfo);

    std::vector<std::shared_ptr<FriendInfo>> friendList;
    ret = friendMgr->getFriendInfoList(friendList);
    CHECK_ERROR(ret)
    for(auto& it: friendList) {
        broadcastList.push_back(it);
    }

    ret = sendDescMessage(broadcastList, chType);
    CHECK_ERROR(ret)

    return 0;
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

MessageManager::FileInfo::FileInfo(const std::string& devId,
                                   const std::string& name,
                                   uint64_t size,
                                   const std::string& md5)
    : mDevId(devId)
    , mName(name)
    , mSize(size)
    , mMd5(md5)
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
    ret = userMgr->getUserInfo(userInfo);
    CHECK_AND_NOTIFY_RETVAL(ret)

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
    CHECK_AND_NOTIFY_RETVAL(ret)

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
    CHECK_AND_NOTIFY_RETVAL(ret)

    std::string jsonStr(msgContent.begin(), msgContent.end());
    Log::W(Log::TAG, "<<<<<<<<<<<<< %s", jsonStr.c_str());
    std::shared_ptr<MessageInfo> cryptoMsgInfo;
    if(jsonStr.find(JsonKey::MessageData) != std::string::npos) { // it's contact sdk
        auto jsonData = Json::parse(jsonStr);
        //auto jsonData = Json::from_cbor(msgContent);
        cryptoMsgInfo = jsonData[JsonKey::MessageData];
    } else { // from other carrier app
        cryptoMsgInfo = MessageManager::MakeTextMessage(jsonStr);
    }

    auto msgInfo = MessageManager::MakeMessage(cryptoMsgInfo);
    if(cryptoMsgInfo->mCryptoAlgorithm.empty() == true
    || cryptoMsgInfo->mCryptoAlgorithm == "plain") {
        msgInfo->mPlainContent = cryptoMsgInfo->mPlainContent;
    } else {
        auto sectyMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mSecurityManager);
        ret = sectyMgr->decryptData(cryptoMsgInfo->mPlainContent, msgInfo->mPlainContent);
        CHECK_AND_NOTIFY_RETVAL(ret)
    }

    std::shared_ptr<HumanInfo> humanInfo;
    if(userMgr->contains(friendCode)) {
        humanInfo = userInfo;
    } else {
        std::shared_ptr<FriendInfo> friendInfo;
        ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
        CHECK_AND_NOTIFY_RETVAL(ret)

        humanInfo = friendInfo;
    }
    if(humanInfo.get() == nullptr) {
        Log::E(Log::TAG, "Failed to get friend info.");
        return;
    }

    if((msgInfo->mType & MessageType::Control) == msgInfo->mType) { // ctrl msg
        ret = msgMgr->processCtrlMessage(humanInfo, humanChType, friendCode, msgInfo);
        CHECK_AND_NOTIFY_RETVAL(ret);
    } else {
        onReceivedMessage(humanInfo, humanChType, msgInfo);
    }
}

void MessageManager::MessageListener::onSentMessage(int msgIndex, int errCode)
{
    Log::W(Log::TAG, "%s", __PRETTY_FUNCTION__);
}

void MessageManager::MessageListener::onFriendRequest(const std::string& friendCode,
                                                      uint32_t channelType,
                                                      const std::string& details)
{
    Log::W(Log::TAG, ">>>>>>>>>>>>> onFriendRequest code:%s, details=%s", friendCode.c_str(), details.c_str());
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

        std::ignore = humanInfo.setHumanInfo(HumanInfo::Item::Did, friendDid);
    } catch(const std::exception& ex) {
        friendDid = "";
        summary = details;
    }
    if(humanChType == ChannelType::Carrier) {
        FriendInfo::CarrierInfo carrierInfo;
        carrierInfo.mUsrId = friendCode;

        ret = humanInfo.addCarrierInfo(carrierInfo, HumanInfo::Status::WaitForAccept);
    } else {
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    }
    CHECK_AND_NOTIFY_RETVAL(ret)

    if(userMgr->contains(friendDid)) {
        std::shared_ptr<UserInfo> userInfo;
        ret = userMgr->getUserInfo(userInfo);
        CHECK_AND_NOTIFY_RETVAL(ret)

        std::string userDid;
        ret = userInfo->getHumanInfo(HumanInfo::Item::Did, userDid);
        CHECK_AND_NOTIFY_RETVAL(ret)

        if(friendDid == userDid) {
            ret = userInfo->mergeHumanInfo(humanInfo, HumanInfo::Status::Offline);
            if(ret == ErrCode::IgnoreMergeOldInfo) {
                Log::W(Log::TAG, "Ignore to add other dev.");
                return;
            }
            CHECK_AND_NOTIFY_RETVAL(ret)

            ret = msgMgr->requestFriend(friendCode, humanChType, "", false);
            CHECK_AND_NOTIFY_RETVAL(ret)
        }
    }

    //if(friendMgr->contains(friendDid)) {
        std::shared_ptr<FriendInfo> friendInfo;
        auto friendStatus = HumanInfo::Status::Offline;
        ret = friendMgr->getFriendInfo(FriendInfo::HumanKind::Did, friendDid, friendInfo);
        if(ret < 0) { // not found
            friendInfo = std::make_shared<FriendInfo>(friendMgr);
            friendMgr->addFriendInfo(friendInfo);
            friendStatus = HumanInfo::Status::WaitForAccept;
        }
        Log::W(Log::TAG, "=============================================== 1");
        ret = friendInfo->mergeHumanInfo(humanInfo, friendStatus);
        if(ret == ErrCode::IgnoreMergeOldInfo) {
            Log::W(Log::TAG, "Ignore to add other dev to friend.");
            return;
        }
        CHECK_AND_NOTIFY_RETVAL(ret)

        Log::W(Log::TAG, ">>>>>>>>>>>>> onFriendRequest() friendStatus=%d", friendStatus);
        if(friendStatus == HumanInfo::Status::Offline) {
            Log::W(Log::TAG, ">>>>>>>>>>>>> onFriendRequest() msgMgr->requestFriend");
            ret = msgMgr->requestFriend(friendCode, humanChType, "", false);
            CHECK_AND_NOTIFY_RETVAL(ret)
        } else {
            Log::W(Log::TAG, ">>>>>>>>>>>>> onFriendRequest() msgMgr->mMessageListener->onFriendRequest");
            msgMgr->mMessageListener->onFriendRequest(friendInfo, humanChType, summary);
        }
    //}
}

void MessageManager::MessageListener::onFriendStatusChanged(const std::string& friendCode,
                                                            uint32_t channelType,
                                                            MessageManager::MessageListener::ChannelStatus status)
{
    int ret = ErrCode::UnknownError;
    ChannelType humanChType = static_cast<ChannelType>(channelType);
    HumanInfo::Status humanStatus = (status == ChannelStatus::Online ? HumanInfo::Status::Online : HumanInfo::Status::Offline);
    std::shared_ptr<HumanInfo> fromHumanInfo;

    auto msgMgr = SAFE_GET_PTR_NO_RETVAL(mMessageManager);
    auto userMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mUserManager);
    auto friendMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mFriendManager);

    std::shared_ptr<UserInfo> userInfo;
    ret = userMgr->getUserInfo(userInfo);
    CHECK_AND_NOTIFY_RETVAL(ret);

    if(userMgr->contains(friendCode)) {
        Log::I(Log::TAG, ">>>>>>>>>>>>> onFriendStatusChanged() is myself");
        std::string userDid;
        UserInfo::CarrierInfo info;
        ret = userInfo->getCarrierInfoByUsrId(friendCode, info);
        CHECK_AND_NOTIFY_RETVAL(ret);

        fromHumanInfo = userInfo;

        UserInfo::Status oldStatus = userInfo->getHumanStatus();
        if(humanChType == ChannelType::Carrier) {
            ret = userInfo->setCarrierStatus(friendCode, humanStatus);
            CHECK_AND_NOTIFY_RETVAL(ret);
        } else {
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
        }
        UserInfo::Status newStatus = userInfo->getHumanStatus();
        if(newStatus != oldStatus) {
            onStatusChanged(userInfo, humanChType, newStatus);
        }
    }

    if(friendMgr->contains(friendCode)) {
        Log::I(Log::TAG, ">>>>>>>>>>>>> onFriendStatusChanged() is friend.");
        std::shared_ptr<FriendInfo> friendInfo;
        ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
        CHECK_AND_NOTIFY_RETVAL(ret);

        fromHumanInfo = friendInfo;
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
        if(oldStatus == FriendInfo::Status::WaitForAccept) {
            ret = friendMgr->cacheFriendToDidChain(friendInfo);
            if(ret < 0) {
                Log::W(Log::TAG, "Failed to cache friend %s to did chain.", friendCode.c_str());
            }
            CHECK_AND_NOTIFY_RETVAL(ret);

            ret = friendMgr->saveLocalData();
            CHECK_AND_NOTIFY_RETVAL(ret);
        }
    }

    if(fromHumanInfo.get() != nullptr) {
        if(humanStatus == HumanInfo::Status::Online) {
            std::vector<std::shared_ptr<HumanInfo>> humanList = {fromHumanInfo};
            ret = msgMgr->sendDescMessage(humanList, humanChType);
            CHECK_AND_NOTIFY_RETVAL(ret);
        }
    } else {
        Log::W(Log::TAG, "onFriendStatusChanged() friendCode=%s is not managed", friendCode.c_str());
    }

    Log::D(Log::TAG, "onFriendStatusChanged() friendCode=%s, status=%d", friendCode.c_str(), humanStatus);
    return;
}

void MessageManager::DataListener::onNotify(const std::string& friendCode,
                                           uint32_t channelType,
                                           const std::string& dataId,
                                           Status status)
{
    Log::W(Log::TAG, ">>>>>>>>>>>>> onNotify code:%s, dataId=%s status=%d", friendCode.c_str(), dataId.c_str(), status);
    auto msgMgr = SAFE_GET_PTR_NO_RETVAL(mMessageManager);
//    auto userMgr = SAFE_GET_PTR(msgMgr->mUserManager);
    auto friendMgr = SAFE_GET_PTR_NO_RETVAL(msgMgr->mFriendManager);

    std::shared_ptr<FriendInfo> friendInfo;
    int ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
    CHECK_RETVAL(ret);

    ChannelType humanChType = static_cast<ChannelType>(channelType);

    msgMgr->mDataListener->onNotify(friendInfo, humanChType, dataId, static_cast<int>(status));
}


int MessageManager::DataListener::onReadData(const std::string& friendCode, uint32_t channelType,
                                             const std::string& dataId,
                                             uint64_t offset, std::vector<uint8_t>& data)
{
    Log::W(Log::TAG, ">>>>>>>>>>>>> onReadData code:%s, dataId=%s", friendCode.c_str(), dataId.c_str());
    auto msgMgr = SAFE_GET_PTR(mMessageManager);
//    auto userMgr = SAFE_GET_PTR(msgMgr->mUserManager);
    auto friendMgr = SAFE_GET_PTR(msgMgr->mFriendManager);

    std::shared_ptr<FriendInfo> friendInfo;
    int ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
    CHECK_ERROR(ret);

    ChannelType humanChType = static_cast<ChannelType>(channelType);

    ret = msgMgr->mDataListener->onReadData(friendInfo, humanChType, dataId, offset, data);
    return ret;
}

int MessageManager::DataListener::onWriteData(const std::string& friendCode, uint32_t channelType,
                                              const std::string& dataId,
                                              uint64_t offset, const std::vector<uint8_t>& data)
{
    Log::W(Log::TAG, ">>>>>>>>>>>>> onWriteData code:%s, dataId=%s", friendCode.c_str(), dataId.c_str());
    auto msgMgr = SAFE_GET_PTR(mMessageManager);
//    auto userMgr = SAFE_GET_PTR(msgMgr->mUserManager);
    auto friendMgr = SAFE_GET_PTR(msgMgr->mFriendManager);

    std::shared_ptr<FriendInfo> friendInfo;
    int ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
    CHECK_ERROR(ret);

    ChannelType humanChType = static_cast<ChannelType>(channelType);

    ret = msgMgr->mDataListener->onWriteData(friendInfo, humanChType, dataId, offset, data);
    return ret;
}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::MakeMessage(std::shared_ptr<MessageManager::MessageInfo> from,
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

int MessageManager::processCtrlMessage(std::shared_ptr<HumanInfo> humanInfo,
                                       ChannelType channelType,
                                       const std::string& friendCode,
                                       const std::shared_ptr<MessageInfo> msgInfo)
{
    if(msgInfo->mType == MessageType::CtrlSyncDesc) {
        std::string humanDesc {msgInfo->mPlainContent.begin(), msgInfo->mPlainContent.end()};
        auto newInfo = HumanInfo();
        int ret = newInfo.HumanInfo::deserialize(humanDesc, true);
        CHECK_ERROR(ret);

        ret = humanInfo->mergeHumanInfo(newInfo, HumanInfo::Status::Online);
        if(ret == ErrCode::IgnoreMergeOldInfo) {
            Log::W(Log::TAG, "Ignore to merge friend desc.");
            return 0;
        }
        CHECK_ERROR(ret);

        mMessageListener->onHumanInfoChanged(humanInfo, channelType);

        auto friendMgr = SAFE_GET_PTR(mFriendManager);
        if(friendMgr->contains(humanInfo)) {
            std::vector<HumanInfo::CarrierInfo> infoArray;
            int ret = humanInfo->getAllCarrierInfo(infoArray);
            if(ret > 0) {
                for(auto& it: infoArray) {
                    ret = requestFriend(it.mUsrAddr, channelType, "", true);
                    if(ret == ErrCode::ChannelFailedFriendExists) {
                        continue;
                    }
                    CHECK_ERROR(ret);
                }
            }
        }


        //if(msgInfo->mType == MessageType::CtrlSyncDesc) {
        //// send latest user desc.
        //std::string humanDesc;
        //ret = userInfo->HumanInfo::serialize(humanDesc, true);
        //if(ret < 0) {
        //Log::E(Log::TAG, "Failed to serialize user human info.");
        //return;
        //}
        //std::vector<uint8_t> humanDescBytes(humanDesc.begin(), humanDesc.end());
        //auto msgInfo = msgMgr->MakeMessage(MessageType::AckSyncDesc, humanDescBytes);
        //ret = msgMgr->sendMessage(humanInfo, humanChType, msgInfo);
        //if(ret < 0) {
        //Log::E(Log::TAG, "Failed to send sync desc message.");
        //return;
        //}
        //}
    } else if(msgInfo->mType == MessageType::CtrlPullData) {
        std::string infoStr {msgInfo->mPlainContent.begin(), msgInfo->mPlainContent.end()};
//        auto jsonInfo = Json::parse(infoStr);
//        std::shared_ptr<FileInfo> fileInfo;
//        from_json(jsonInfo, fileInfo);

        auto it = mMessageChannelMap.find(channelType);
        auto channel = it->second;

        int ret = channel->sendData(friendCode, infoStr);
        std::vector<uint8_t> dataRetBytes({(uint8_t)(ret < 0 ? -1 : 0)});
        auto msgInfo = MakeMessage(MessageType::CtrlPullDataAck, dataRetBytes);
        Json jsonData = Json::object();
        jsonData[JsonKey::MessageData] = msgInfo;
        std::string jsonStr = jsonData.dump();
        std::vector<uint8_t> data(jsonStr.begin(), jsonStr.end());
        std::ignore = channel->sendMessage(friendCode, data); // ignore to check ack

        CHECK_ERROR(ret); // check sendData result
    } else if(msgInfo->mType == MessageType::CtrlPullDataAck) {
        int ret = (int8_t)msgInfo->mPlainContent[0];
        auto status = (ret == 0 ? DataListener::Status::PeerInitialized : DataListener::Status::PeerFailed);
        mDataListener->onNotify(humanInfo, channelType, "", static_cast<int>(status));
    }

    return 0;
}

int MessageManager::sendDescMessage(const std::vector<std::shared_ptr<HumanInfo>>& humanList, ChannelType chType)
{
    // send latest user desc.
    auto userMgr = SAFE_GET_PTR(mUserManager);
    std::shared_ptr<UserInfo> userInfo;
    int ret = userMgr->getUserInfo(userInfo);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to process send desc message, user info is not exists.");
        return ret;
    }

    std::string humanDesc;
    ret = userInfo->HumanInfo::serialize(humanDesc, true);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to serialize user human info.");
        return ret;
    }
    std::vector<uint8_t> humanDescBytes(humanDesc.begin(), humanDesc.end());
    auto msgInfo = MakeMessage(MessageType::CtrlSyncDesc, humanDescBytes);

    for(const auto& it: humanList) {
        std::string humanCode;
        it->getHumanCode(humanCode);

        ret = sendMessage(it, chType, msgInfo, false);
        Log::I(Log::TAG, "Failed to send sync desc message to %s. ret=%d", humanCode.c_str(), ret);
        if(ret < 0) {
            continue;
        }
    }

    return 0;
}

} // namespace elastos
