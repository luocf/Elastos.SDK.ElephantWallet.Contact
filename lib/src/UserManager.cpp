//
//  UserManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <UserManager.hpp>

#include "BlkChnClient.hpp"
#include <CompatibleFileSystem.hpp>
#include <DateTime.hpp>
#include <Log.hpp>
#include <MessageManager.hpp>
#include <SafePtr.hpp>
#include <Platform.hpp>
#include <iostream>
#include <sstream>
#include <Json.hpp>


namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
UserManager::UserManager(std::weak_ptr<SecurityManager> sectyMgr)
    : mSecurityManager(sectyMgr)
    , mMessageManager()
    , mConfig()
    , mUserListener()
    , mUserInfo()
{
}

UserManager::~UserManager()
{
}

void UserManager::setUserListener(std::shared_ptr<UserListener> listener)
{
    mUserListener = listener;
}

void UserManager::setConfig(std::weak_ptr<Config> config, std::weak_ptr<MessageManager> msgMgr)
{
    mConfig = config;
    mMessageManager = msgMgr;
}

int UserManager::loadLocalData()
{
    auto config = SAFE_GET_PTR(mConfig);
    auto dataFilePath = std::filesystem::path(config->mUserDataDir.c_str()) / DataFileName;

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::vector<uint8_t> originData;
    int ret = sectyMgr->loadCryptoFile(dataFilePath.string(), originData);
    if(ret < 0) {
        return ret;
    }

    std::string userData {originData.begin(), originData.end()};
    try {
        ret = mUserInfo->deserialize(userData);
        if(ret < 0) {
            return ret;
        }
    } catch(const std::exception& ex) {
        Log::E(Log::TAG, "Failed to load local data from: %s.\nex=%s", dataFilePath.c_str(), ex.what());
        return ErrCode::JsonParseException;
    }

    Log::I(Log::TAG, "Success to load local data from: %s.", dataFilePath.c_str());
    return 0;
}

int UserManager::saveLocalData()
{
    std::string userData;
    int ret = serialize(userData);
    if(ret < 0) {
        return ret;
    }

    auto config = SAFE_GET_PTR(mConfig);
    auto dataFilePath = std::filesystem::path(config->mUserDataDir.c_str()) / DataFileName;

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::vector<uint8_t> originData {userData.begin(), userData.end()};
    ret = sectyMgr->saveCryptoFile(dataFilePath.string(), originData);
    if(ret < 0) {
        return ret;
    }

    Log::D(Log::TAG, "Save local data to: %s, data: %s", dataFilePath.c_str(), userData.c_str());

    return 0;
}

int UserManager::serialize(std::string& value) const
{
    value = "";

    std::string userData;
    int ret = mUserInfo->serialize(userData);
    if(ret < 0) {
        return ret;
    }

    value = userData;
    return 0;
}

int UserManager::restoreUserInfo()
{
    mUserInfo = std::make_shared<UserInfo>(weak_from_this());

    int ret = loadLocalData();
    if(ret == 0) {
        Log::I(Log::TAG, "UserManager::makeUser() Success to recover user from local.");
    } else if(ret == ErrCode::FileNotExistsError) {
        Log::I(Log::TAG, "UserManager::makeUser() Local user info not exists.");
        ret = syncDidChainData();
        if(ret == 0) {
            Log::I(Log::TAG, "UserManager::makeUser() Success to recover user from did chain.");
        } else if(ret == ErrCode::BlkChnEmptyPropError) {
            Log::I(Log::TAG, "UserManager::makeUser() Can't find user info from local or did chain.");
            ret = ErrCode::EmptyInfoError;
        }
    }
    if(ret < 0) {
        Log::W(Log::TAG, "UserManager::makeUser() Failed to restore user, ret=%d", ret);
        return ret;
    }

    return 0;
}

int UserManager::newUserInfo()
{
    Log::V(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mUserInfo = std::make_shared<UserInfo>(weak_from_this());

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::string pubKey;
    int ret = sectyMgr->getPublicKey(pubKey);
    if(ret < 0) {
        return ret;
    }
    mUserInfo->setHumanInfo(UserInfo::Item::ChainPubKey, pubKey);

    std::string currDevId;
    ret = Platform::GetCurrentDevId(currDevId);
    if(ret < 0) {
        return ret;
    }

    std::string currDevName;
    ret = Platform::GetCurrentDevName(currDevName);
    if(ret < 0) {
        return ret;
    }

    auto msgMgr = SAFE_GET_PTR(mMessageManager);

    std::weak_ptr<MessageChannelStrategy> weakChCarrier;
    ret = msgMgr->getChannel(MessageManager::ChannelType::Carrier, weakChCarrier);
    if(ret < 0) {
        return ret;
    }
    auto chCarrier = SAFE_GET_PTR(weakChCarrier);

    std::string carrierSecKey;
    ret = chCarrier->getSecretKey(carrierSecKey);
    if(ret < 0) {
        return ret;
    }

    ret = mUserInfo->setIdentifyCode(UserInfo::Type::CarrierSecKey, carrierSecKey);
    if(ret < 0) {
        return ret;
    }

    std::string carrierAddr;
    ret = chCarrier->getAddress(carrierAddr);
    if(ret < 0) {
        return ret;
    }

    HumanInfo::CarrierInfo carrierInfo{carrierAddr, "", {currDevId, currDevName, DateTime::CurrentMS()}};
    ret = mUserInfo->addCarrierInfo(carrierInfo, UserInfo::Status::Offline);
    if(ret < 0) {
        return ret;
    }

    ret = mUserInfo->getCarrierInfoByDevId(currDevId, carrierInfo);
    if(ret < 0) {
        return ret;
    }

    std::string carrierInfoStr;
    ret = mUserInfo->serialize(carrierInfo, carrierInfoStr);
    if(ret < 0) {
        return ret;
    }

    std::string identifyCodeStr;
    ret = mUserInfo->IdentifyCode::serialize(identifyCodeStr, true, false);
    if(ret < 0) {
        return ret;
    }

    auto bcClient = BlkChnClient::GetInstance();
    ret = bcClient->cacheDidProp("PublicKey", pubKey);
    if(ret < 0) {
        return ret;
    }
    ret = bcClient->cacheDidProp("CarrierID", carrierInfoStr);
    if(ret < 0) {
        return ret;
    }
    ret = bcClient->cacheDidProp("IdentifyCode", identifyCodeStr);
    if(ret < 0) {
        return ret;
    }

    Log::V(Log::TAG, "%s new carrier info: %s", __PRETTY_FUNCTION__, carrierInfoStr.c_str());

    return 0;
}


int UserManager::getUserInfo(std::shared_ptr<UserInfo>& userInfo)
{
    if(mUserInfo.get() == nullptr) {
        return ErrCode::NotReadyError;
    }

    userInfo = mUserInfo;

    return 0;
}

bool UserManager::contains(const std::string& userCode)
{
    return mUserInfo->contains(userCode);
}

bool UserManager::contains(const std::shared_ptr<HumanInfo>& userInfo)
{
    return mUserInfo->contains(userInfo);
}

int UserManager::syncDidChainData()
{
    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::string did;
    int ret = sectyMgr->getDid(did);
    if(ret < 0) {
        return ret;
    }

    auto bcClient = BlkChnClient::GetInstance();

    std::vector<std::string> identifyCodeArray;
    ret = bcClient->downloadDidPropHistory(did, "IdentifyCode", identifyCodeArray);
    if(ret < 0) {
        Log::W(Log::TAG, "UserManager::syncDidChainData() Failed to download IdentifyCode.");
    }
    for(const auto& it: identifyCodeArray) {
        IdentifyCode identifyCode;
        ret = identifyCode.deserialize(it);
        if (ret < 0) {
            Log::W(Log::TAG, "UserManager::syncDidChainData() Failed to deserialize IdentifyCode: %s.", it.c_str());
            continue;
        }

        ret = mUserInfo->mergeIdentifyCode(identifyCode);
        if (ret < 0) {
            Log::W(Log::TAG, "UserManager::syncDidChainData() Failed to merge IdentifyCode: %s.", it.c_str());
            continue;
        }
    }

    auto humanInfo = std::make_shared<HumanInfo>();
    ret = bcClient->downloadHumanInfo(did, humanInfo);
    if(ret < 0) {
        Log::W(Log::TAG, "UserManager::syncDidChainData() Failed to download HumanInfo.");
        return ret;
    }

    ret = mUserInfo->mergeHumanInfo(*humanInfo, HumanInfo::Status::Offline);
    if(ret < 0) {
        Log::W(Log::TAG, "UserManager::syncDidChainData() Failed to merge HumanInfo.");
        return ret;
    }

    return 0;
}

int UserManager::monitorDidChainData()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
    // std::string did;
    // int ret = sectyMgr->getDid(did);
    // if(ret < 0) {
    //     return ret;
    // }

    // auto callback = std::function<void(int errcode, const std::string& keyPath, const std::string& result)>;

    // auto bcClient = BlkChnClient::GetInstance();
    // int ret = bcClient->getDidPropHistoryPath(did, "CarrierID")

    //     int ret = appendMoniter(const std::string& path,
    //                             const MonitorCallback& callback);
    // //     if(ret < 0) {
    // //         return ret;
    // //     }

    // //     return 0;

}

// int UserManager::uploadUserInfo()
// {
//     auto bcClient = BlkChnClient::GetInstance();

//     int ret = bcClient->uploadHumanInfo(mUserInfo);
//     if(ret < 0) {
//         return ret;
//     }

//     return 0;
// }

int UserManager::setupMultiDevChannels()
{
    auto msgMgr = SAFE_GET_PTR(mMessageManager);

    std::vector<HumanInfo::CarrierInfo> carrierInfoArray;
    int ret = mUserInfo->getAllCarrierInfo(carrierInfoArray);
    if(ret < 0) {
        return ret;
    }

    for(const auto& carrierInfo: carrierInfoArray) {
        int ret = msgMgr->requestFriend(carrierInfo.mUsrAddr,
                                        MessageManager::ChannelType::Carrier,
                                        "", true);
        Log::I(Log::TAG, "UserManager::setupMultiDevChannels() add %s", carrierInfo.mUsrAddr.c_str());
        if(ret < 0
        && ret != ErrCode::ChannelFailedFriendSelf
        && ret != ErrCode::ChannelFailedFriendExists) {
            Log::W(Log::TAG, "UserManager::setupMultiDevChannels() Failed to add %s ret=%d", carrierInfo.mUsrAddr.c_str(), ret);
        }
    }

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
