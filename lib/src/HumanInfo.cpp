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
#include <DateTime.hpp>
#include <Log.hpp>
#include <SecurityManager.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
struct JsonKey {
    static constexpr const char* HumanInfo = "HumanInfo";

    static constexpr const char* BoundCarrierArray = "BoundCarrierArray";
    static constexpr const char* BoundCarrierStatus = "BoundCarrierStatus";
    static constexpr const char* CommonInfoMap = "CommonInfoMap";
    static constexpr const char* StatusMap = "StatusMap";
    static constexpr const char* Status = "Status";
    static constexpr const char* HumanCode = "HumanCode";
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

bool HumanInfo::contains(const std::string& humanCode)
{
    auto kind = AnalyzeHumanKind(humanCode);

    int ret = ErrCode::UnknownError;
    switch(kind) {
    case HumanKind::Did:
        {
            std::string info;
            ret = getHumanInfo(Item::Did, info);
        }
        break;
    case HumanKind::Ela:
        {
            std::string info;
            ret = getHumanInfo(Item::ElaAddress, info);
        }
        break;
    case HumanKind::Carrier:
        {
            CarrierInfo info;
            ret = getCarrierInfoByUsrId(humanCode, info);
        }
        break;
    default:
        break;
    }

    return (ret >=0 ? true : false);
}

bool HumanInfo::contains(const std::shared_ptr<HumanInfo>& humanInfo)
{
    std::string info;
    int ret = humanInfo->getHumanInfo(Item::Did, info);
    if(ret >= 0) {
        return contains(info);
    }

    ret = humanInfo->getHumanInfo(Item::ElaAddress, info);
    if(ret >= 0) {
        return contains(info);
    }

    std::vector<CarrierInfo> infoArray;
    ret = humanInfo->getAllCarrierInfo(infoArray);
    for(const auto& info: infoArray) {
        bool find = contains(info.mUsrId);
        if(find == true) {
            return true;
        }
    }

    return false;
}

int HumanInfo::getHumanCode(std::string& humanCode) const
{
    // 1. did friend
    int ret = getHumanInfo(Item::Did, humanCode);
    if(ret == 0) {
        return 0;
    }

    // 2. carrier friend
    auto carrierSize = mBoundCarrierArray.size();
    assert(carrierSize <= 1); // only did friend can own multi carrier info
    if (carrierSize > 0) {
        const auto& carrierInfo = mBoundCarrierArray[0];
        humanCode = carrierInfo.mUsrAddr;
        if (humanCode.empty() == true) {
            humanCode = carrierInfo.mUsrId;
        }

        if (humanCode.empty() == false) {
            return 0;
        }
    }

    // 3. ela friend
    ret = getHumanInfo(Item::ElaAddress, humanCode);
    if(ret == 0) {
        return 0;
    }

    assert(false); // never reached
    return ErrCode::NotFoundError;
}

int HumanInfo::addCarrierInfo(const HumanInfo::CarrierInfo& info, const HumanInfo::Status status)
{
    Log::D(Log::TAG, " ============    %s", __PRETTY_FUNCTION__);
    if(info.mUsrAddr.empty() == true
    && info.mUsrId.empty() == true) {
        return ErrCode::InvalidArgument;
    }

    HumanInfo::CarrierInfo correctedInfo = info;

    if(correctedInfo.mDevInfo.mDevId.empty() == true) {
        auto datetime = DateTime::Current();
        correctedInfo.mDevInfo.mDevId = datetime;
    }
    if(correctedInfo.mDevInfo.mDevName.empty() == true) {
        correctedInfo.mDevInfo.mDevId = "Unknown";
    }

    if(correctedInfo.mUsrAddr.empty() == false) {
        int ret = ChannelImplCarrier::GetCarrierUsrIdByAddress(correctedInfo.mUsrAddr, correctedInfo.mUsrId);
        CHECK_ERROR(ret)
    }
    if(info.mUsrId.empty() == false
    && info.mUsrId != correctedInfo.mUsrId) {
        return ErrCode::ConflictWithExpectedError;
    }

    for(auto idx = 0; idx < mBoundCarrierArray.size(); idx++) {
        auto& existsInfo = mBoundCarrierArray[idx];

//        if(existsInfo.mDevInfo.mDevId.empty() == false
//        && existsInfo.mDevInfo.mDevId != correctedInfo.mDevInfo.mDevId) { // not changed
//            continue;
//        }

        //if(existsInfo.mUsrId != correctedInfo.mUsrId) {
            //continue;
        //}

        // found info by usrId
        if(existsInfo.mDevInfo.mDevId == correctedInfo.mDevInfo.mDevId) {
            if(existsInfo.mDevInfo.mUpdateTime >= correctedInfo.mDevInfo.mUpdateTime) { // not changed
                Log::D(Log::TAG, " HumanInfo::addCarrierInfo() IgnoreMergeOldInfo: exists=%s input=%s",
                                 existsInfo.mDevInfo.mDevId.c_str(), correctedInfo.mDevInfo.mDevId.c_str());
                return ErrCode::IgnoreMergeOldInfo;
            } else { // update info
                existsInfo = correctedInfo;
                return idx;
            }
        } else if(existsInfo.mDevInfo.mDevId == "Unknown"
        && existsInfo.mUsrId == correctedInfo.mUsrId) { // found same dev
            existsInfo = correctedInfo;
            return idx;
        }
    }

    mBoundCarrierArray.push_back(correctedInfo); // if not exists, add new one
    mBoundCarrierStatus.push_back(status);
    return mBoundCarrierArray.size();
}

int HumanInfo::getCarrierInfoByUsrId(const std::string& usrId, HumanInfo::CarrierInfo& info) const
{
    info = {"", "", {"", "", 0}};
    for(auto idx = 0; idx < mBoundCarrierArray.size(); idx++) {
        if(mBoundCarrierArray[idx].mUsrId == usrId) {
            info = mBoundCarrierArray[idx];
            return idx;
        }
    }

    return ErrCode::NotFoundError;
}

int HumanInfo::getCarrierInfoByDevId(const std::string& devId, CarrierInfo& info) const
{
    info = {"", "", {"", "", 0}};
    for(auto idx = 0; idx < mBoundCarrierArray.size(); idx++) {
        if(mBoundCarrierArray[idx].mDevInfo.mDevId == devId) {
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

    mBoundCarrierStatus[idx] = status;
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

    Log::D(Log::TAG, "%s %d %s=>%s", __PRETTY_FUNCTION__, item, mCommonInfoMap[item].c_str(), value.c_str());

    if(item == Item::ChainPubKey) {
        std::string expectedDid, expectedElaAddr;
        int ret = SecurityManager::GetDid(value, expectedDid);
        CHECK_ERROR(ret)
        ret = HumanInfo::setHumanInfo(Item::Did, expectedDid);
        CHECK_ERROR(ret)

        ret = SecurityManager::GetElaAddress(value, expectedElaAddr);
        CHECK_ERROR(ret)
        ret = HumanInfo::setHumanInfo(Item::ElaAddress, expectedElaAddr);
        CHECK_ERROR(ret)
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
    auto it = value.mCommonInfoMap.find(Item::ChainPubKey);
    if(it != value.mCommonInfoMap.end() && it->second.empty() == false) {
        if(this->mCommonInfoMap[Item::ChainPubKey].empty() == false
           && this->mCommonInfoMap[Item::ChainPubKey] != it->second) {
            return ErrCode::MergeInfoFailed;
        }

        this->mCommonInfoMap[Item::ChainPubKey] = it->second;
    }

    it = value.mCommonInfoMap.find(Item::Did);
    if(it != value.mCommonInfoMap.end() && it->second.empty() == false) {
        if(this->mCommonInfoMap[Item::Did].empty() == false
        && this->mCommonInfoMap[Item::Did] != it->second) {
            return ErrCode::MergeInfoFailed;
        }

        this->mCommonInfoMap[Item::Did] = it->second;
    }

    it = value.mCommonInfoMap.find(Item::ElaAddress);
    if(it != value.mCommonInfoMap.end() && it->second.empty() == false) {
        if(this->mCommonInfoMap[Item::ElaAddress].empty() == false
        && this->mCommonInfoMap[Item::ElaAddress] != it->second) {
            return ErrCode::MergeInfoFailed;
        }

        this->mCommonInfoMap[Item::ElaAddress] = it->second;
    }

    Log::D(Log::TAG, " ============ 0   %s mBoundCarrierArray:%d", __PRETTY_FUNCTION__, value.mBoundCarrierArray.size());
    for(const auto& it: value.mBoundCarrierArray) {
        int ret = addCarrierInfo(it, status);
        if(ret == ErrCode::IgnoreMergeOldInfo) {
            Log::W(Log::TAG, "Contact::monitorDidChainCarrierID() Ignore to sync CarrierId: %s", it.mUsrId.c_str());
            continue;
        }
        CHECK_ERROR(ret)
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

    auto it = mStatusMap.find(kind);
    if(it == mStatusMap.end()) {
        return ErrCode::NotFoundError;
    }

    status = it->second;
    return 0;
}

int HumanInfo::setHumanStatus(const Status from, const Status to)
{
    for(auto& it: mStatusMap) {
        if(it.second == from) {
            it.second = to;
        }
    }

    for(auto& it: mBoundCarrierStatus) {
        if(it == from) {
            it = to;
        }
    }

    return 0;
}

HumanInfo::Status HumanInfo::getHumanStatus() const
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

inline void to_json(Json& j, const HumanInfo::CarrierInfo::DeviceInfo& info) {
    j = Json {
        {"DevId", info.mDevId},
        {"DevName", info.mDevName},
        {"UpdateTime", info.mUpdateTime},
    };
}

inline void from_json(const Json& j, HumanInfo::CarrierInfo::DeviceInfo& info) {
    info.mDevId = j["DevId"];
    info.mDevName = j["DevName"];
    info.mUpdateTime = j["UpdateTime"];
}

inline void to_json(Json& j, const HumanInfo::CarrierInfo& info) {
    j = Json {
        {"CarrierAddr", info.mUsrAddr},
        {"CarrierId", info.mUsrId},
        {"DeviceInfo", info.mDevInfo},
    };
}

inline void from_json(const Json& j, HumanInfo::CarrierInfo& info) {
    info.mUsrAddr = j["CarrierAddr"];
    info.mUsrId = j["CarrierId"];
    info.mDevInfo = j["DeviceInfo"];
}

NLOHMANN_JSON_SERIALIZE_ENUM(
    HumanInfo::Status,
    {
        {HumanInfo::Status::Invalid, "Invalid"},
        {HumanInfo::Status::WaitForAccept, "WaitForAccept"},
        {HumanInfo::Status::Offline, "Offline"},
        {HumanInfo::Status::Online, "Online"},
        {HumanInfo::Status::Removed, "Removed"},
    }
);

int HumanInfo::serialize(const CarrierInfo& info, std::string& value)
{
    Json jsonInfo= Json(info);
    value = jsonInfo.dump();
    return 0;
}

int HumanInfo::deserialize(const std::string& value, CarrierInfo& info)
{
    Json jsonInfo= Json::parse(value);
    info = jsonInfo.get<CarrierInfo>();
    return 0;
}

int HumanInfo::serialize(std::string& value, bool summaryOnly) const
{
    Json jsonInfo = Json::object();

    jsonInfo[JsonKey::CommonInfoMap] = mCommonInfoMap;
    jsonInfo[JsonKey::BoundCarrierArray] = mBoundCarrierArray;
    //if(summaryOnly == true) {
        //for(auto& carrierInfo: jsonInfo[JsonKey::BoundCarrierArray]) {
            //carrierInfo.erase("CarrierId");
            //carrierInfo.erase("DeviceInfo");
        //}
    //}
    if(summaryOnly == false) {
        jsonInfo[JsonKey::BoundCarrierStatus] = mBoundCarrierStatus;
        jsonInfo[JsonKey::StatusMap] = mStatusMap;
    }

    value = jsonInfo.dump();

    return 0;
}

int HumanInfo::deserialize(const std::string& value, bool summaryOnly)
{
    Log::D(Log::TAG, "HumanInfo::deserialize() value=%s, summaryOnly=%d", value.c_str(), summaryOnly);
    Json jsonInfo;
    try {
        jsonInfo= Json::parse(value);
    } catch(Json::parse_error) {
        return ErrCode::JsonParseException;
    }

    mCommonInfoMap = jsonInfo[JsonKey::CommonInfoMap].get<std::map<Item, std::string>>();
    mBoundCarrierArray = jsonInfo[JsonKey::BoundCarrierArray].get<std::vector<CarrierInfo>>();

    if(summaryOnly == false) {
        mBoundCarrierStatus = jsonInfo[JsonKey::BoundCarrierStatus].get<std::vector<Status>>();
        mStatusMap = jsonInfo[JsonKey::StatusMap].get<std::map<HumanKind, Status>>();
    }

    return 0;
}

int HumanInfo::toJson(std::shared_ptr<Json>& value) const
{
    if(value.get() == nullptr) {
        return ErrCode::InvalidArgument;
    }

    std::string humanCode;
    int ret = getHumanCode(humanCode);
    CHECK_ERROR(ret);

    Json jsonInfo;
    jsonInfo[JsonKey::CommonInfoMap] = mCommonInfoMap;
    jsonInfo[JsonKey::BoundCarrierArray] = mBoundCarrierArray;
    jsonInfo[JsonKey::Status] = getHumanStatus();
    jsonInfo[JsonKey::HumanCode] = humanCode;

    (*value)[JsonKey::HumanInfo] = jsonInfo;

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
