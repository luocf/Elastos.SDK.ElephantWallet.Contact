//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactBridge.hpp>
#include <SafePtr.hpp>
#include <base/src/JsonDefine.hpp>

#include "Log.hpp"
#include "Json.hpp"
#include "ContactMessage.hpp"

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/


/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ContactBridge::ContactBridge()
        : mContactImpl()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mContactImpl = elastos::Contact::Factory::Create();
}
ContactBridge::~ContactBridge()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    elastos::ErrCode::SetErrorListener(nullptr);
}

void ContactBridge::setListener(CrossBase* listener)
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mListener = dynamic_cast<ContactListener*>(listener);
//    mListener->onCallback(0, nullptr);

    auto errorListener = std::bind(&ContactListener::onError, mListener,
                                   std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    elastos::ErrCode::SetErrorListener(errorListener);

    auto sectyListener = mListener->getSecurityListener();
    auto msgListener = mListener->getMessageListener();
    mContactImpl->setListener(sectyListener, nullptr, nullptr, msgListener);

    return;
}

int ContactBridge::start()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    int ret = mContactImpl->start();

    return ret;
}

int ContactBridge::stop()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    int ret = mContactImpl->stop();

    return ret;
}

int ContactBridge::setUserInfo(int item, const char* value)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \

    int ret = userMgr->setUserInfo(static_cast<elastos::UserInfo::Item>(item), value);
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::setIdentifyCode(int type, const char* value)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \

    int ret = userMgr->setIdentifyCode(static_cast<elastos::UserInfo::Type>(type), value);
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::getHumanInfo(const char* humanCode, std::stringstream* info)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    if(humanCode == nullptr) {
        return elastos::ErrCode::InvalidArgument;
    }

    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \
    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    int ret = elastos::ErrCode::UnknownError;
    std::shared_ptr<elastos::HumanInfo> humanInfo;
    if(std::string("-user-info-") == humanCode
    || userMgr->contains(humanCode) == true) {
        std::shared_ptr<elastos::UserInfo> userInfo;
        ret = userMgr->getUserInfo(userInfo);
        CHECK_ERROR(ret);
        humanInfo = userInfo;
    } else if (friendMgr->contains(humanCode) == true) {
        std::shared_ptr<elastos::FriendInfo> friendInfo;
        ret = friendMgr->tryGetFriendInfo(humanCode, friendInfo);
        CHECK_ERROR(ret);
        humanInfo = friendInfo;
    } else {
        return elastos::ErrCode::NotFoundError;
    }

    auto jsonInfo = std::make_shared<elastos::Json>();
    ret = humanInfo->toJson(jsonInfo);
    CHECK_ERROR(ret);

    info->str(jsonInfo->dump());
    return 0;
}

int ContactBridge::getHumanStatus(const char* humanCode)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \
    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    std::shared_ptr<elastos::HumanInfo> humanInfo;
    if(std::string("-user-info-") == humanCode
    || userMgr->contains(humanCode) == true) {
        std::shared_ptr<elastos::UserInfo> userInfo;
        int ret = userMgr->getUserInfo(userInfo);
        CHECK_ERROR(ret);
        humanInfo = userInfo;
    } else if (friendMgr->contains(humanCode) == true) {
        std::shared_ptr<elastos::FriendInfo> friendInfo;
        int ret = friendMgr->tryGetFriendInfo(humanCode, friendInfo);
        CHECK_ERROR(ret);
        humanInfo = friendInfo;
    } else {
        return elastos::ErrCode::NotFoundError;
    }

    auto status = humanInfo->getHumanStatus();

    return static_cast<int>(status);
}

int ContactBridge::addFriend(const char* friendCode, const char* summary)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    int ret = friendMgr->tryAddFriend(friendCode, summary, true, true);
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::removeFriend(const char* friendCode)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    int ret = friendMgr->tryRemoveFriend(friendCode);
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::acceptFriend(const char* friendCode)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    int ret = friendMgr->tryAcceptFriend(friendCode);
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::getFriendList(std::stringstream* info)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    std::vector<std::shared_ptr<elastos::FriendInfo>> friendList;
    int ret = friendMgr->getFriendInfoList(friendList);
    CHECK_ERROR(ret);

    elastos::Json friendJsonArray = elastos::Json::array();
    for(const auto& it: friendList) {
        auto jsonInfo = std::make_shared<elastos::Json>();
        ret = it->toJson(jsonInfo);
        CHECK_ERROR(ret);

        friendJsonArray.push_back(*jsonInfo);
    }

    info->str(friendJsonArray.dump());
    return 0;
}

int ContactBridge::sendMessage(const char* friendCode, int chType, CrossBase* message)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto msgInfo = dynamic_cast<ContactMessage*>(message);
    if(msgInfo == nullptr) {
        return elastos::ErrCode::InvalidArgument;
    }

    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \
    auto weakMsgMgr = mContactImpl->getMessageManager();
    auto msgMgr =  SAFE_GET_PTR(weakMsgMgr);                                                                      \

    std::shared_ptr<elastos::FriendInfo> friendInfo;
    int ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
    CHECK_ERROR(ret);

    ret = msgMgr->sendMessage(friendInfo, static_cast<elastos::MessageManager::ChannelType>(chType), msgInfo->mMessageInfo);
    CHECK_ERROR(ret);

    return ret;
}

int ContactBridge::pullData(const char* friendCode, int chType,
                            const char* devId, const char* fileInfo)
{
    if(fileInfo == nullptr) {
        return elastos::ErrCode::InvalidArgument;
    }

    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \
    auto weakMsgMgr = mContactImpl->getMessageManager();
    auto msgMgr =  SAFE_GET_PTR(weakMsgMgr);                                                                      \

    std::shared_ptr<elastos::FriendInfo> friendInfo;
    int ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
    CHECK_ERROR(ret);

    ret = msgMgr->pullData(friendInfo, static_cast<elastos::MessageManager::ChannelType>(chType),
                           devId, fileInfo);
    CHECK_ERROR(ret);

    return ret;
}

int ContactBridge::syncInfoDownloadFromDidChain()
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    int ret = mContactImpl->syncInfoDownloadFromDidChain();
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::syncInfoUploadToDidChain()
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    int ret = mContactImpl->syncInfoUploadToDidChain();
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::setWalletAddress(const char* name, const char* value)
{
    if(mContactImpl->isStarted() == false) {
        return elastos::ErrCode::NotReadyError;
    }

    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \

    int ret = userMgr->setWalletAddress(name, value);
    CHECK_ERROR(ret);

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
