//
//  IdentifyCode.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <IdentifyCode.hpp>

#include <Json.hpp>
#include <Log.hpp>
#include <Platform.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
struct JsonKey {
    static constexpr const char* IdentifyCode = "IdentifyCode";

    static constexpr const char* CarrierSecretKeyMap = "CarrierSecretKeyMap";
    static constexpr const char* IdCodeMap = "IdCodeMap";
};


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
IdentifyCode::IdentifyCode()
    : mCarrierSecretKeyMap()
    , mIdCodeMap()
    , mUpdateTime()
{
}

IdentifyCode::~IdentifyCode()
{
}

int IdentifyCode::setIdentifyCode(Type type, const std::string& value)
{
    switch(type) {
    case Type::CarrierSecKey:
    {
        std::string devId;
        int ret = Platform::GetCurrentDevId(devId);
        CHECK_ERROR(ret)
        mCarrierSecretKeyMap[devId] = value;
        break;
    }
    default:
        mIdCodeMap[type] = value;
        break;
    }

    return 0;
}

int IdentifyCode::getIdentifyCode(Type type, std::string& value) const
{

    switch(type) {
    case Type::CarrierSecKey:
    {
        std::string devId;
        int ret = Platform::GetCurrentDevId(devId);
        CHECK_ERROR(ret)
        const auto& it = mCarrierSecretKeyMap.find(devId);
        if(it == mCarrierSecretKeyMap.end()) {
            return ErrCode::NotFoundError;
        }
        value = it->second;
        break;
    }
    default:
        const auto& it = mIdCodeMap.find(type);
        if(it == mIdCodeMap.end()) {
            return ErrCode::NotFoundError;
        }
        value = it->second;
        break;
    }

    return 0;
}

int IdentifyCode::mergeIdentifyCode(const IdentifyCode& value)
{
    for(const auto& it: value.mIdCodeMap) {
        int ret = setIdentifyCode(it.first, it.second);
        if(ret < 0) {
            Log::W(Log::TAG, "IdentifyCode::mergeIdentifyCode() Failed to merge %d: %s", it.first, it.second.c_str());
        }
    }

    std::string carrierSecKey;
    int ret = value.getIdentifyCode(Type::CarrierSecKey, carrierSecKey);
    if(ret == 0) {
        ret = setIdentifyCode(Type::CarrierSecKey, carrierSecKey);
        if(ret < 0) {
            Log::W(Log::TAG, "IdentifyCode::mergeIdentifyCode() Failed to merge carrier secret key.");
        }
    }
    
    return 0;
}

int IdentifyCode::serialize(std::string& value,
                            bool withCarrierSecKey,
                            bool withIdCode) const
{
    Json jsonInfo = Json::object();

    if(withCarrierSecKey == true) {
        jsonInfo[JsonKey::CarrierSecretKeyMap] = mCarrierSecretKeyMap;
    }
    if(withIdCode == true) {
        jsonInfo[JsonKey::IdCodeMap] = mIdCodeMap;
    }

    value = jsonInfo.dump();

    return 0;
}

int IdentifyCode::deserialize(const std::string& value)
{
    Json jsonInfo;
    try {
        jsonInfo= Json::parse(value);
    } catch(Json::parse_error) {
        return ErrCode::JsonParseException;
    }

    if(jsonInfo.find(JsonKey::CarrierSecretKeyMap) != jsonInfo.end()) {
        std::string devId;
        int ret = Platform::GetCurrentDevId(devId);
        CHECK_ERROR(ret)

        auto carrierSecretKeyMap = jsonInfo[JsonKey::CarrierSecretKeyMap].get<std::map<std::string, std::string>>();
        const auto& it = carrierSecretKeyMap.find(devId);
        if(it != carrierSecretKeyMap.end()) {
            mCarrierSecretKeyMap[devId] = it->second;
        }
    }
    if(jsonInfo.find(JsonKey::IdCodeMap) != jsonInfo.end()) {
        mIdCodeMap = jsonInfo[JsonKey::IdCodeMap].get<std::map<Type, std::string>>();
    }

    return 0;
}

int IdentifyCode::toJson(std::shared_ptr<Json>& value) const
{
    if(value.get() == nullptr) {
        return ErrCode::InvalidArgument;
    }

    Json jsonInfo;
    jsonInfo[JsonKey::CarrierSecretKeyMap] = mCarrierSecretKeyMap;
    jsonInfo[JsonKey::IdCodeMap] = mIdCodeMap;

    (*value)[JsonKey::IdentifyCode] = jsonInfo;

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
