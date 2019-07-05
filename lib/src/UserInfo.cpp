//
//  UserInfo.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <UserInfo.hpp>
#include <UserManager.hpp>

#include <Log.hpp>
#include <Json.hpp>
#include <Platform.hpp>
#include <SafePtr.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
struct JsonKey {
    static constexpr const char* HumanInfo = "HumanInfo";
    static constexpr const char* IdentifyCode = "IdentifyCode";
};


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
UserInfo::UserInfo(std::weak_ptr<UserManager> userMgr)
    : HumanInfo()
    , IdentifyCode()
    , mUserManager(userMgr)
{
}

UserInfo::~UserInfo()
{
}

int UserInfo::serialize(const CarrierInfo& info, std::string& value) const
{
    int ret = HumanInfo::serialize(info, value);
    return ret;
}

int UserInfo::deserialize(const std::string& value, CarrierInfo& info) const
{
    int ret = HumanInfo::deserialize(value, info);
    return ret;
}

int UserInfo::serialize(std::string& value, bool summaryOnly) const
{
    Json jsonInfo = Json::object();

    std::string humanInfo;
    int ret = HumanInfo::serialize(humanInfo, summaryOnly);
    if(ret < 0) {
        return ret;
    }
    jsonInfo[JsonKey::HumanInfo] = humanInfo;

    if(summaryOnly == false) {
        std::string identCode;
        ret = IdentifyCode::serialize(identCode);
        if(ret < 0) {
            return ret;
        }
        jsonInfo[JsonKey::IdentifyCode] = identCode;
    }

    value = jsonInfo.dump();

    return 0;
}

int UserInfo::deserialize(const std::string& value, bool summaryOnly)
{
    Json jsonInfo= Json::parse(value);

    std::string humanInfo = jsonInfo[JsonKey::HumanInfo];
    int ret = HumanInfo::deserialize(humanInfo, summaryOnly);
    if(ret < 0) {
        return ret;
    }

    if(summaryOnly == false) {
        std::string identCode = jsonInfo[JsonKey::IdentifyCode];
        ret = IdentifyCode::deserialize(identCode);
        if(ret < 0) {
            return ret;
        }
    }

    return 0;

}

int UserInfo::addCarrierInfo(const CarrierInfo& info, const Status status)
{
    int ret = HumanInfo::addCarrierInfo(info, status);
    if(ret <= 0) { // error or not changed
        return ret;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int UserInfo::setHumanInfo(Item item, const std::string& value)
{
    int ret = HumanInfo::setHumanInfo(item, value);
    Log::D(Log::TAG, "%s ret=%d", __PRETTY_FUNCTION__, ret);
    if(ret <= 0) { // error or not changed
        return ret;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int UserInfo::mergeHumanInfo(const HumanInfo& value, const Status status)
{
    int ret = HumanInfo::mergeHumanInfo(value, status);
    Log::D(Log::TAG, "%s ret=%d", __PRETTY_FUNCTION__, ret);
    if(ret < 0) { // error or not changed
        return ret;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int UserInfo::setIdentifyCode(Type type, const std::string& value)
{
    int ret = IdentifyCode::setIdentifyCode(type, value);
    if(ret <= 0) { // error or not changed
        return ret;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    if(ret < 0) {
        return ret;
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
