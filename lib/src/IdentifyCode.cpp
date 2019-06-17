//
//  IdentifyCode.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <IdentifyCode.hpp>

#include <Json.hpp>
#include <Platform.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
struct JsonKey {
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
    : mIdCodeMap()
{
}

IdentifyCode::~IdentifyCode()
{
}

int IdentifyCode::setIdentifyCode(Type type, const std::string& value)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int IdentifyCode::getIdentifyCode(Type type, std::string& value) const
{
    //int ret;

    //switch(type) {
    //case Type::CarrierKey:
        //std::string devId;
        //ret = GetCurrentDevId(devId);
        //if(ret < 0) {
            //return ret;
        //}
        //value = 

    //}


    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int IdentifyCode::serialize(std::string& value) const
{
    Json jsonInfo = Json::object();

    jsonInfo[JsonKey::CarrierSecretKeyMap] = mCarrierSecretKeyMap;
    jsonInfo[JsonKey::IdCodeMap] = mIdCodeMap;

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

    mCarrierSecretKeyMap = jsonInfo[JsonKey::CarrierSecretKeyMap].get<std::map<std::string, std::string>>();
    mIdCodeMap = jsonInfo[JsonKey::IdCodeMap].get<std::map<Type, std::string>>();

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
