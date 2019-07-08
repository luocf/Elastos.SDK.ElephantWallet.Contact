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
    Log::D(Log::TAG, "set log level: %d", level);
    Log::SetLevel(static_cast<Log::Level>(level));
}

int Contact::Factory::SetLocalDataDir(const std::string& dir)
{
    if(dir.empty()) {
        return ErrCode::InvalidArgument;
    }

    std::error_code stdErrCode;
    bool ret = elastos::filesystem::create_directories(dir, stdErrCode);
    if(ret == false
    || stdErrCode.value() != 0) {
        int errCode = ErrCode::StdSystemErrorIndex - stdErrCode.value();
        auto errMsg = ErrCode::ToString(errCode);
        Log::D(Log::TAG, "Failed to set local data dir, errcode: %s", errMsg.c_str());
        return errCode;
    }

    sLocalDataDir = dir;
    Log::D(Log::TAG, "set local data dir: %d", sLocalDataDir.c_str());

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

    ret = mUserManager->restoreUserInfo();
    if(ret < 0
    && ret != ErrCode::EmptyInfoError) {
        return ret;
    }
    bool successLoadInfo = (ret != ErrCode::EmptyInfoError);
    bool successLoadCarrier = false;

    std::shared_ptr<UserInfo> userInfo;
    mUserManager->getUserInfo(userInfo);
    std::string carrierSecKey;
    ret = userInfo->getIdentifyCode(UserInfo::Type::CarrierSecKey, carrierSecKey);
    if (ret == 0) {
        successLoadCarrier = true;
    }

    ret = mMessageManager->presetChannels(mConfig);
    if(ret < 0) {
        return ret;
    }

    if(successLoadInfo == false || successLoadCarrier == false) {
        ret = mUserManager->newUserInfo(successLoadInfo == true);
        if (ret < 0) {
            return ret;
        }
    }

    ret = mFriendManager->restoreFriendsInfo();
    if(ret < 0) {
        return ret;
    }

    ret = mMessageManager->openChannels();
    if(ret < 0) {
        return ret;
    }

    ret = mUserManager->monitorDidChainData();
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int Contact::syncInfoDownloadFromDidChain()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int Contact::syncInfoUploadToDidChain()
{
    auto bcClient = BlkChnClient::GetInstance();
    if(bcClient.get() == nullptr) {
        return ErrCode::NotReadyError;
    }

    int ret = bcClient->uploadCachedDidProp();
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
    bool isDir = elastos::filesystem::is_directory(Factory::sLocalDataDir);
    if(isDir == false) {
        return ErrCode::InvalidLocalDataDir;
    }
    auto perms = elastos::filesystem::status(Factory::sLocalDataDir).permissions();
    if((perms & elastos::filesystem::perms::owner_read) == elastos::filesystem::perms::none
    || (perms & elastos::filesystem::perms::owner_write) == elastos::filesystem::perms::none) {
        return ErrCode::InvalidLocalDataDir;
    }

    std::string did;
    int ret = mSecurityManager->getDid(did);
    if(ret < 0) {
        return ret;
    }

    auto userDataDir = elastos::filesystem::path(Factory::sLocalDataDir) / did;
    std::error_code stdErrCode;
    bool bret = elastos::filesystem::create_directories(userDataDir, stdErrCode);
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

} // namespace elastos
