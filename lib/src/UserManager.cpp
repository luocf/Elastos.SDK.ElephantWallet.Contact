//
//  UserManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <UserManager.hpp>

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
    , mUserListener()
    , mUserInfo()
    , mStatus(Status::Pending)
{
}

UserManager::~UserManager()
{
}

void UserManager::setUserListener(std::shared_ptr<UserListener> listener)
{
    mUserListener = listener;
}

int UserManager::makeUser()
{
    std::string did;
    int ret = mSecurityManager.lock()->getDid(did);
    if(ret < 0) {
        return ret;
    }

    mUserInfo = std::make_shared<UserInfo>(did);
    setStatus(Status::Ready);

    return 0;
}

UserManager::Status UserManager::getStatus()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int UserManager::updateUserInfo(IdentifyCode::Type idType, const std::string& value)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

std::weak_ptr<UserInfo> UserManager::getUserInfo() const
{
    return mUserInfo;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
void UserManager::setStatus(Status status)
{
    bool changed = (mStatus != status);
    mStatus = status;
    if(changed == true
    && mUserListener != nullptr) {
        mUserListener->onStatusChanged(mUserInfo, mStatus);
    }
}

} // namespace elastos
