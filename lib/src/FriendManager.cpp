//
//  FriendManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <FriendManager.hpp>

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
FriendManager::FriendManager(std::weak_ptr<SecurityManager> sectyMgr)
    : mSecurityManager(sectyMgr)
    , mFriendListener()
    , mFriendList()
{
}

FriendManager::~FriendManager()
{
}

void FriendManager::setFriendListener(std::shared_ptr<FriendListener> listener)
{
    mFriendListener = listener;
}

int FriendManager::addFriendByDid(std::string did)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::addFriendByCarrier(std::string carrierAddress)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::addFriendByEla(std::string elaAddress)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::removeFriendByDid(std::string did)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::removeFriendByCarrier(std::string carrierAddress)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::removeFriendByEla(std::string elaAddress)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

std::string FriendManager::findFriend(int type, std::string content)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

std::vector<FriendInfo> FriendManager::filterFriends(std::string regex)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
