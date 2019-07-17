//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactBridge.hpp>
#include <SafePtr.hpp>

#include "Log.hpp"
#include "Json.hpp"

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
}

void ContactBridge::setListener(CrossBase* listener)
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mListener = dynamic_cast<ContactListener*>(listener);
//    mListener->onCallback(0, nullptr);

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

int ContactBridge::getHumanInfo(const char* humanCode, std::stringstream* info)
{
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
        humanInfo = userInfo;
    } else if (friendMgr->contains(humanInfo) == true) {
        std::shared_ptr<elastos::FriendInfo> friendInfo;
        ret = friendMgr->tryGetFriendInfo(humanCode, friendInfo);
        humanInfo = friendInfo;
    } else {
        return elastos::ErrCode::NotFoundError;
    }
    CHECK_ERROR(ret);

    auto jsonInfo = std::make_shared<elastos::Json>();
    ret = humanInfo->toJson(jsonInfo);
    CHECK_ERROR(ret);

    info->str(jsonInfo->dump());
    return 0;
}

int ContactBridge::getHumanStatus(const char* humanCode)
{
    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \

    std::shared_ptr<elastos::UserInfo> userInfo;
    int ret = userMgr->getUserInfo(userInfo);
    CHECK_ERROR(ret);

    elastos::HumanInfo::Status status;
    if(userInfo->contains(humanCode) == true) {
        status = userInfo->getHumanStatus();
    } else {
        return elastos::ErrCode::UnimplementedError;
    }

    return static_cast<int>(status);
}

int ContactBridge::addFriend(const char* friendCode, const char* summary)
{
    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    int ret = friendMgr->tryAddFriend(friendCode, summary);
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::acceptFriend(const char* friendCode)
{
    auto weakFriendMgr = mContactImpl->getFriendManager();
    auto friendMgr =  SAFE_GET_PTR(weakFriendMgr);                                                                      \

    int ret = friendMgr->tryAcceptFriend(friendCode);
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::getFriendList(std::stringstream* info)
{
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


int ContactBridge::syncInfoDownloadFromDidChain()
{
    int ret = mContactImpl->syncInfoDownloadFromDidChain();
    CHECK_ERROR(ret);

    return 0;
}

int ContactBridge::syncInfoUploadToDidChain()
{
    int ret = mContactImpl->syncInfoUploadToDidChain();
    CHECK_ERROR(ret);

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/