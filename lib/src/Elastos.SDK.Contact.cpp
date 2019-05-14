//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <Elastos.SDK.Contact.hpp>

#include <CompatibleFileSystem.hpp>
#include <Log.hpp>
#include <BlkChnClient.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
std::string Contact::Factory::sLocalDataDir;


/***********************************************/
/***** static function implement ***************/
/***********************************************/
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

    ret = mMessageManager->openChannels(mConfig);
    if(ret < 0) {
        return ret;
    }

    ret = mUserManager->makeUser();
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
    , mMessageManager(std::make_shared<MessageManager>(mSecurityManager))
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

    auto cfgFilePath = std::filesystem::path(userDataDir.c_str()) / "config.dat";
    mConfig = std::make_shared<Config>(cfgFilePath);
    ret = mConfig->load();
    if(ret < 0) {
        return ret;
    }

    ret = BlkChnClient::InitInstance(mConfig, mSecurityManager);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

} // namespace elastos
