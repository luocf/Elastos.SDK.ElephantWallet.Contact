//
//  HumanInfo.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <HumanInfo.hpp>

#include "ChannelImplCarrier.hpp"
#include <ctime>
#include <Json.hpp>
#include <DataTime.hpp>
#include <Log.hpp>
#include <SecurityManager.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
struct JsonKey {
    static constexpr const char* BoundCarrierArray = "BoundCarrierArray";
    static constexpr const char* BoundCarrierStatus = "BoundCarrierStatus";
    static constexpr const char* CommonInfoMap = "CommonInfoMap";
    static constexpr const char* StatusMap = "StatusMap";
};


/***********************************************/
/***** static function implement ***************/
/***********************************************/
HumanInfo::HumanKind HumanInfo::AnalyzeHumanKind(const std::string& code)
{
    auto kind = static_cast<HumanInfo::HumanKind>(ErrCode::InvalidArgument);

    if(SecurityManager::IsValidDid(code) == true) {
        kind = HumanInfo::HumanKind::Did;
    } else if(SecurityManager::IsValidElaAddress(code) == true) {
        kind = HumanInfo::HumanKind::Ela;
    } else if(ChannelImplCarrier::IsValidCarrierAddress(code) == true) {
        kind = HumanInfo::HumanKind::Carrier;
    } else if(ChannelImplCarrier::IsValidCarrierUsrId(code) == true) {
        kind = HumanInfo::HumanKind::Carrier;
    }

    return kind;
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
HumanInfo::HumanInfo()
    : mBoundCarrierArray()
    , mBoundCarrierStatus()
    , mCommonInfoMap()
    , mStatusMap()
{
}

HumanInfo::~HumanInfo()
{
}

int HumanInfo::addCarrierInfo(const HumanInfo::CarrierInfo& info, const HumanInfo::Status status)
{
    if(info.mUsrAddr.empty() == true
    && info.mUsrId.empty() == true) {
        return ErrCode::InvalidArgument;
    }

    HumanInfo::CarrierInfo correctedInfo = info;

    if(correctedInfo.mDevId.empty() == true) {
        auto datatime = DataTime::Current();
        correctedInfo.mDevId = "unknown-" + datatime;
    }

    if(correctedInfo.mUsrAddr.empty() == false) {
        int ret = ChannelImplCarrier::GetCarrierUsrIdByAddress(correctedInfo.mUsrAddr, correctedInfo.mUsrId);
        if(ret < 0) {
            return ret;
        }
    }
    if(info.mUsrId.empty() == false
    && info.mUsrId != correctedInfo.mUsrId) {
        return ErrCode::ConflictWithExpectedError;
    }

    for(auto idx = 0; idx < mBoundCarrierArray.size(); idx++) {
        auto& existsInfo = mBoundCarrierArray[idx];
        if(existsInfo.mUsrId != correctedInfo.mUsrId) {
            continue;
        }

        // found info by usrId
        if(existsInfo.mDevId == correctedInfo.mDevId
        && existsInfo.mUsrAddr == correctedInfo.mUsrAddr) { // not changed
            return 0;
        } else { // update info
            existsInfo = correctedInfo;
            return idx;
        }
    }

    mBoundCarrierArray.push_back(correctedInfo); // if not exists, add new one
    mBoundCarrierStatus.push_back(status);
    return (mBoundCarrierArray.size() - 1);
}

int HumanInfo::getCarrierInfoByUsrId(const std::string& usrId, HumanInfo::CarrierInfo& info) const
{
    info = {"", "", ""};
    for(auto idx = 0; idx < mBoundCarrierArray.size(); idx++) {
        if(mBoundCarrierArray[idx].mUsrId == usrId) {
            info = mBoundCarrierArray[idx];
            return idx;
        }
    }

    return ErrCode::NotFoundError;
}

int HumanInfo::getAllCarrierInfo(std::vector<HumanInfo::CarrierInfo>& infoArray) const
{
    infoArray = mBoundCarrierArray;
    return mBoundCarrierArray.size();
}

int HumanInfo::setCarrierStatus(const std::string& usrId, const Status status)
{
    int idx;
    for(idx = 0; idx < mBoundCarrierArray.size(); idx++) {
        if(mBoundCarrierArray[idx].mUsrId == usrId) {
            break;
        }
    }
    if(idx >= mBoundCarrierArray.size()) {
        return ErrCode::NotFoundError;
    }

    //mBoundCarrierStatus[idx] = status;
    return 0;
}

int HumanInfo::getCarrierStatus(const std::string& usrId, Status& status) const
{
    int idx;
    for(idx = 0; idx < mBoundCarrierArray.size(); idx++) {
        if(mBoundCarrierArray[idx].mUsrId == usrId) {
            break;
        }
    }
    if(idx >= mBoundCarrierArray.size()) {
        return ErrCode::NotFoundError;
    }

    status = mBoundCarrierStatus[idx];
    return 0;
}

int HumanInfo::setHumanInfo(Item item, const std::string& value)
{
    if(mCommonInfoMap[item] == value) {
        return 0;
    }

    Log::D(Log::TAG, "%s %s=>%s", __PRETTY_FUNCTION__, mCommonInfoMap[item].c_str(), value.c_str());

    if(item == Item::ChainPubKey) {
        std::string expectedDid, expectedElaAddr;
        int ret = SecurityManager::GetDid(value, expectedDid);
        if(ret < 0) {
            return ret;
        }
        ret = HumanInfo::setHumanInfo(Item::Did, expectedDid);
        if(ret < 0) {
            return ret;
        }

        ret = SecurityManager::GetElaAddress(value, expectedElaAddr);
        if(ret < 0) {
            return ret;
        }
        ret = HumanInfo::setHumanInfo(Item::ElaAddress, expectedElaAddr);
        if(ret < 0) {
            return ret;
        }
    }

    mCommonInfoMap[item] = value;

    return static_cast<int>(item);
}

int HumanInfo::getHumanInfo(Item item, std::string& value) const
{
    value = "";

    const auto& it = mCommonInfoMap.find(item);
    if(it == mCommonInfoMap.end()) {
        return ErrCode::NotFoundError;
    }

    value = it->second;

    return 0;
}

int HumanInfo::mergeHumanInfo(const HumanInfo& value, const Status status)
{
    auto it = value.mCommonInfoMap.find(Item::Did);
    if(it != value.mCommonInfoMap.end() && it->second.empty() == false) {
        if(this->mCommonInfoMap[Item::Did].empty() == false
        && this->mCommonInfoMap[Item::Did] == it->second) {
            return ErrCode::MergeInfoFailed;
        }

        this->mCommonInfoMap[Item::Did] = it->second;
    }

    it = value.mCommonInfoMap.find(Item::ElaAddress);
    if(it != value.mCommonInfoMap.end() && it->second.empty() == false) {
        if(this->mCommonInfoMap[Item::ElaAddress].empty() == false
        && this->mCommonInfoMap[Item::ElaAddress] == it->second) {
            return ErrCode::MergeInfoFailed;
        }

        this->mCommonInfoMap[Item::ElaAddress] = it->second;
    }

    for(const auto& it: value.mBoundCarrierArray) {
        int ret = addCarrierInfo(it, status);
        if(ret < 0) {
            return ret;
        }
    }

    return 0;
}

int HumanInfo::setHumanStatus(HumanInfo::HumanKind kind, const HumanInfo::Status status)
{
    if(kind == HumanInfo::HumanKind::Carrier) {
        return ErrCode::InvalidArgument;
    }

    mStatusMap[kind] = status;
    return 0;
}

int HumanInfo::getHumanStatus(HumanInfo::HumanKind kind, HumanInfo::Status& status)
{
    if(kind == HumanInfo::HumanKind::Carrier) {
        return ErrCode::InvalidArgument;
    }

    status = mStatusMap[kind];
    return 0;
}

HumanInfo::Status HumanInfo::getHumanStatus()
{
    Status status = Status::Invalid;

    for(const auto& it: mStatusMap) {
        if(static_cast<int>(status) < static_cast<int>(it.second)) {
            status = it.second;
        }
    }

    for(const auto& it: mBoundCarrierStatus) {
        if(static_cast<int>(status) < static_cast<int>(it)) {
            status = it;
        }
    }

    return status;
}

inline void to_json(Json& j, const HumanInfo::CarrierInfo& info) {
    j = Json {
        {"DevId", info.mDevId},
        {"UsrAddr", info.mUsrAddr},
        {"UsrId", info.mUsrId},
    };
}

inline void from_json(const Json& j, HumanInfo::CarrierInfo& info) {
    info.mDevId = j["DevId"];
    info.mUsrAddr = j["UsrAddr"];
    info.mUsrId = j["UsrId"];
}

NLOHMANN_JSON_SERIALIZE_ENUM(HumanInfo::Status, {
    { HumanInfo::Status::Invalid, "Invalid"},
    { HumanInfo::Status::WaitForAccept, "WaitForAccept"},
    { HumanInfo::Status::Offline, "Offline"},
    { HumanInfo::Status::Online, "Offline"}, // Online alse save as OffLine
});

int HumanInfo::serialize(std::string& value, bool summaryOnly) const
{
    Json jsonInfo = Json::object();

    jsonInfo[JsonKey::CommonInfoMap] = mCommonInfoMap;
    if(summaryOnly == false) {
        jsonInfo[JsonKey::BoundCarrierArray] = mBoundCarrierArray;
        jsonInfo[JsonKey::BoundCarrierStatus] = mBoundCarrierStatus;
        jsonInfo[JsonKey::StatusMap] = mStatusMap;
    }

    value = jsonInfo.dump();

    return 0;
}

int HumanInfo::deserialize(const std::string& value, bool summaryOnly)
{
    Json jsonInfo;
    try {
        jsonInfo= Json::parse(value);
    } catch(Json::parse_error) {
        return ErrCode::JsonParseException;
    }

    mCommonInfoMap = jsonInfo[JsonKey::CommonInfoMap].get<std::map<Item, std::string>>();
    if(summaryOnly == false) {
        mBoundCarrierArray = jsonInfo[JsonKey::BoundCarrierArray].get<std::vector<CarrierInfo>>();
        mBoundCarrierStatus = jsonInfo[JsonKey::BoundCarrierStatus].get<std::vector<Status>>();
        mStatusMap = jsonInfo[JsonKey::StatusMap].get<std::map<HumanKind, Status>>();
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
