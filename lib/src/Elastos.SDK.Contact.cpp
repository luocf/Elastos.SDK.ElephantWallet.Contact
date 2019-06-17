//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <Elastos.SDK.Contact.hpp>

#include "BlkChnClient.hpp"
#include "ChannelImplCarrier.hpp"
#include "CompatibleFileSystem.hpp"
#include "DateTime.hpp"
#include "Log.hpp"
#include "Platform.hpp"
#include "SafePtr.hpp"


namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
std::string Contact::Factory::sLocalDataDir;

/***********************************************/
/***** static function implement ***************/
/***********************************************/

void Contact::Factory::SetLogLevel(int level)
{
    Log::SetLevel(static_cast<Log::Level>(level));
}

int Contact::Factory::SetLocalDataDir(const std::string& dir)
{
    if(dir.empty()) {
        return ErrCode::InvalidArgument;
    }

    std::error_code stdErrCode;
    bool ret = std::filesystem::create_directories(dir, stdErrCode);
    if(ret == false
    || stdErrCode.value() != 0) {
        int errCode = ErrCode::StdSystemErrorIndex - stdErrCode.value();
        auto errMsg = ErrCode::ToString(errCode);
        Log::D(Log::TAG, "Failed to set local data dir, errcode: %s", errMsg.c_str());
        return errCode;
    }

    sLocalDataDir = dir;

    return 0;
}

std::shared_ptr<Contact> Contact::Factory::Create()
{
    struct Impl: Contact {
    };

    return std::make_shared<Impl>();
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
void Contact::setListener(std::shared_ptr<SecurityManager::SecurityListener> sectyListener,
                          std::shared_ptr<UserManager::UserListener> userListener,
                          std::shared_ptr<FriendManager::FriendListener> friendListener,
                          std::shared_ptr<MessageManager::MessageListener> msgListener)
{
    mSecurityManager->setSecurityListener(sectyListener);
    mUserManager->setUserListener(userListener);
    mFriendManager->setFriendListener(friendListener);
    mMessageManager->setMessageListener(msgListener);
}

int Contact::start()
{
    int ret = initGlobal();
    if(ret < 0) {
        return ret;
    }

    ret = mUserManager->makeUser();
    if(ret < 0) {
        return ret;
    }

    ret = mMessageManager->presetChannels(mConfig);
    if(ret < 0) {
        return ret;
    }

    ret = setUserInfo();
    if(ret < 0) {
        return ret;
    }

    ret = mFriendManager->restoreFriends();
    if(ret < 0) {
        return ret;
    }

    ret = mMessageManager->openChannels();
    if(ret < 0) {
        return ret;
    }


    return 0;
}

std::weak_ptr<SecurityManager> Contact::getSecurityManager()
{
    return mSecurityManager;
}

std::weak_ptr<UserManager> Contact::getUserManager()
{
    return mUserManager;
}

std::weak_ptr<FriendManager> Contact::getFriendManager()
{
    return mFriendManager;
}

std::weak_ptr<MessageManager> Contact::getMessageManager()
{
    return mMessageManager;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
Contact::Contact()
    : mSecurityManager(std::make_shared<SecurityManager>())
    , mUserManager(std::make_shared<UserManager>(mSecurityManager))
    , mFriendManager(std::make_shared<FriendManager>(mSecurityManager))
    , mMessageManager(std::make_shared<MessageManager>(mSecurityManager, mUserManager, mFriendManager))
    , mConfig()
{
}

Contact::~Contact()
{
}


int Contact::getUserDataDir(std::string& dir)
{
    if(Factory::sLocalDataDir.empty()) {
        return ErrCode::InvalidLocalDataDir;
    }
    bool isDir = std::filesystem::is_directory(Factory::sLocalDataDir);
    if(isDir == false) {
        return ErrCode::InvalidLocalDataDir;
    }
    auto perms = std::filesystem::status(Factory::sLocalDataDir).permissions();
    if((perms & std::filesystem::perms::owner_read) == std::filesystem::perms::none
    || (perms & std::filesystem::perms::owner_write) == std::filesystem::perms::none) {
        return ErrCode::InvalidLocalDataDir;
    }

    std::string did;
    int ret = mSecurityManager->getDid(did);
    if(ret < 0) {
        return ret;
    }

    auto userDataDir = std::filesystem::path(Factory::sLocalDataDir) / did;
    std::error_code stdErrCode;
    bool bret = std::filesystem::create_directories(userDataDir, stdErrCode);
    if(bret == false
    || stdErrCode.value() != 0) {
        int errCode = ErrCode::StdSystemErrorIndex - stdErrCode.value();
        auto errMsg = ErrCode::ToString(errCode);
        Log::D(Log::TAG, "Failed to set user data dir, errcode: %s", errMsg.c_str());
        return errCode;
    }

    dir = userDataDir.string();

    return 0;
}

int Contact::initGlobal()
{
    int ret;

    std::string userDataDir;
    ret = getUserDataDir(userDataDir);
    if(ret < 0) {
        return ret;
    }
    Log::D(Log::TAG, "%s userdatadir:%s", __PRETTY_FUNCTION__, userDataDir.c_str());

    mConfig = std::make_shared<Config>(userDataDir);
    ret = mConfig->load();
    if(ret < 0) {
        return ret;
    }

    mUserManager->setConfig(mConfig, mMessageManager);
    mFriendManager->setConfig(mConfig, mMessageManager);

    ret = BlkChnClient::InitInstance(mConfig, mSecurityManager);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int Contact::setUserInfo()
{
    std::shared_ptr<UserInfo> userInfo;
    int ret = mUserManager->getUserInfo(userInfo);
    if(ret < 0) {
        return ret;
    }

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

    std::weak_ptr<MessageChannelStrategy> weakChCarrier;
    ret = mMessageManager->getChannel(MessageManager::ChannelType::Carrier, weakChCarrier);
    if(ret < 0) {
        return ret;
    }
    auto chCarrier = SAFE_GET_PTR(weakChCarrier);

    std::string carrierAddr;
    ret = chCarrier->getAddress(carrierAddr);
    if(ret < 0) {
        return ret;
    }

    HumanInfo::CarrierInfo info{carrierAddr, "", {currDevId, currDevName, DateTime::CurrentMS()}};
    ret = userInfo->addCarrierInfo(info, UserInfo::Status::Offline);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

} // namespace elastos
