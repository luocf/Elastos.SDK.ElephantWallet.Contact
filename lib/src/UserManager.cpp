//
//  UserManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <UserManager.hpp>

#include "BlkChnClient.hpp"
#include <CompatibleFileSystem.hpp>
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

int UserManager::makeUser()
{
    mUserInfo = std::make_shared<UserInfo>(weak_from_this());

    int ret = loadLocalData();
    if(ret == 0) {
        Log::I(Log::TAG, "UserManager::makeUser() Success to recover user from local.");
    } else if(ret == ErrCode::FileNotExistsError) {
        Log::I(Log::TAG, "UserManager::makeUser() Local user info not exists.");
        ret = syncUserInfo();
        if(ret == 0) {
            Log::I(Log::TAG, "UserManager::makeUser() Success to recover user from did chain.");
        } else if(ret == ErrCode::BlkChnEmptyPropError) {
            Log::I(Log::TAG, "UserManager::makeUser() Can't find user info from local or did chain, make a new user.");
            ret = 0;
        }
    }
    if(ret < 0) {
        Log::E(Log::TAG, "UserManager::makeUser() Failed to recover user, ret=%d", ret);
        return ret;
    }

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::string pubKey, did, elaAddr;
    ret = sectyMgr->getPublicKey(pubKey);
    if(ret < 0) {
        return ret;
    }
    mUserInfo->setHumanInfo(UserInfo::Item::ChainPubKey, pubKey);

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

int UserManager::syncUserInfo()
{
    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::string did;
    int ret = sectyMgr->getDid(did);
    if(ret < 0) {
        return ret;
    }

    auto bcClient = BlkChnClient::GetInstance();

    auto humanInfo = std::make_shared<HumanInfo>();
    ret = bcClient->downloadHumanInfo(did, humanInfo);
    if(ret < 0) {
        return ret;
    }

    ret = mUserInfo->mergeHumanInfo(*humanInfo, HumanInfo::Status::Offline);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int UserManager::uploadUserInfo()
{
    auto bcClient = BlkChnClient::GetInstance();

    int ret = bcClient->uploadHumanInfo(mUserInfo);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

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
