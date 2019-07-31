#ifndef _JSON_DEFINE_HPP_
#define _JSON_DEFINE_HPP_

#include <Json.hpp>

#include <HumanInfo.hpp>
#include <MessageManager.hpp>

namespace elastos {

/***********************************************/
/***** macro definition ************************/
/***********************************************/
struct JsonKey {
    // HumanInfo
    static constexpr const char* HumanInfo = "HumanInfo";
    static constexpr const char* BoundCarrierArray = "BoundCarrierArray";
    static constexpr const char* BoundCarrierStatus = "BoundCarrierStatus";
    static constexpr const char* CommonInfoMap = "CommonInfoMap";
    static constexpr const char* StatusMap = "StatusMap";
    static constexpr const char* Status = "Status";
    static constexpr const char* HumanCode = "HumanCode";

    // FriendInfo
    static constexpr const char* Alias = "Alias";

    // IdentifyCode
    static constexpr const char* IdentifyCode = "IdentifyCode";
    static constexpr const char* CarrierSecretKeyMap = "CarrierSecretKeyMap";
    static constexpr const char* IdCodeMap = "IdCodeMap";

    // MessageInfo
    static constexpr const char* Did = "Did";
    static constexpr const char* Summary = "Summary";
    static constexpr const char* MessageData = "MessageData";
};

// HumanInfo
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

// MessageInfo
NLOHMANN_JSON_SERIALIZE_ENUM(
    MessageManager::MessageType,
    {
        {MessageManager::MessageType::MsgText, "MsgText"},
        {MessageManager::MessageType::MsgAudio, "MsgAudio"},
        {MessageManager::MessageType::MsgTransfer, "MsgTransfer"},

        {MessageManager::MessageType::CtrlSyncDesc, "CtrlSyncDesc"},
    }
);

inline void to_json(Json& j, const std::shared_ptr<MessageManager::MessageInfo>& info) {
    j = Json {
        {"Type", info->mType},
        {"PlainContent", info->mPlainContent},
        {"CryptoAlgorithm", info->mCryptoAlgorithm},
        {"TimeStamp", info->mTimeStamp},
    };
}

inline void from_json(const Json& j, std::shared_ptr<MessageManager::MessageInfo>& info) {
    info = MessageManager::MakeEmptyMessage();
    info->mType = j["Type"];
    info->mPlainContent = j["PlainContent"].get<std::vector<uint8_t>>();
    info->mCryptoAlgorithm = j["CryptoAlgorithm"];
    info->mTimeStamp = j["TimeStamp"];
}

} //namespace elastos

#endif /* _JSON_DEFINE_HPP_ */

