//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <Contact.hpp>
#include <SafePtr.hpp>

#include "Log.hpp"

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/


/***********************************************/
/***** class public function implement  ********/
/***********************************************/
Contact::Contact()
        : mContactImpl()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mContactImpl = elastos::Contact::Factory::Create();
}
Contact::~Contact()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}

void Contact::setListener(CrossBase* listener)
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mListener = dynamic_cast<ContactListener*>(listener);
//    mListener->onCallback(0, nullptr);

    auto sectyListener = mListener->getSecurityListener();
    auto msgListener = mListener->getMessageListener();
    mContactImpl->setListener(sectyListener, nullptr, nullptr, msgListener);

    return;
}

int Contact::start()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    int ret = mContactImpl->start();

    return ret;
}

int Contact::getHumanInfo(const char* humanCode, std::stringstream* info)
{
    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \

    int ret = elastos::ErrCode::UnknownError;
    std::shared_ptr<elastos::HumanInfo> humanInfo;
    if(std::string("-user-info-") == humanCode) {
        std::shared_ptr<elastos::UserInfo> userInfo;
        ret = userMgr->getUserInfo(userInfo);
        humanInfo = userInfo;
    } else {
        return elastos::ErrCode::UnimplementedError;
    }
    if(ret < 0) {
        return ret;
    }

    std::string infoStr;
    ret = humanInfo->HumanInfo::serialize(infoStr, true);
    if(ret < 0) {
        return ret;
    }

    info->str(infoStr);
    return 0;
}

int Contact::getHumanStatus(const char* humanCode)
{
    auto weakUserMgr = mContactImpl->getUserManager();
    auto userMgr =  SAFE_GET_PTR(weakUserMgr);                                                                      \

    std::shared_ptr<elastos::UserInfo> userInfo;
    int ret = userMgr->getUserInfo(userInfo);
    if(ret < 0) {
        return ret;
    }

    elastos::HumanInfo::Status status;
    if(userInfo->contains(humanCode) == true) {
        status = userInfo->getHumanStatus();
    } else {
        return elastos::ErrCode::UnimplementedError;
    }

    return static_cast<int>(status);
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
