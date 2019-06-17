//
//  ChannelImplCarrier.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ChannelImplCarrier.hpp>

#include <cstring>
#include <Log.hpp>
#include <SafePtr.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/
bool ChannelImplCarrier::IsValidCarrierAddress(const std::string& address)
{
    bool valid = ela_address_is_valid(address.c_str());
    return valid;
}

bool ChannelImplCarrier::IsValidCarrierUsrId(const std::string& usrId)
{
    bool valid = ela_id_is_valid(usrId.c_str());
    return valid;
}

int ChannelImplCarrier::GetCarrierUsrIdByAddress(const std::string& address, std::string& usrId)
{
    char buf[ELA_MAX_ID_LEN + 1] = {0};
    auto ret = ela_get_id_by_address(address.c_str(), buf, sizeof(buf));
    if(ret == nullptr) {
        int err = ela_get_error();
        char strerr_buf[512] = {0};
        ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
        Log::E(Log::TAG, "Failed to get carrier usr id from:%s! ret=%s(0x%x)", address.c_str(), strerr_buf, err);
        return ErrCode::ChannelFailedCarrier;
    }

    usrId = ret;
    return 0;
}


/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ChannelImplCarrier::ChannelImplCarrier(uint32_t chType,
                                       std::shared_ptr<ChannelListener> listener,
                                       std::weak_ptr<Config> config)
    : MessageChannelStrategy(chType, listener)
    , mConfig(config)
    , mCarrier()
    , mTaskThread()
    , mChannelStatus(ChannelListener::ChannelStatus::Pending)
    , mRecvDataCache()
{
}

ChannelImplCarrier::~ChannelImplCarrier()
{
}

int ChannelImplCarrier::preset(const std::string& profile)
{
    if(mCarrier != nullptr) {
        return ErrCode::ChannelFailedMultiOpen;
    }

    auto config = SAFE_GET_PTR(mConfig);

    ElaOptions carrierOpts;
    ElaCallbacks carrierCallbacks;

    memset(&carrierOpts, 0, sizeof(carrierOpts));
    memset(&carrierCallbacks, 0, sizeof(carrierCallbacks));
    carrierCallbacks.connection_status = OnCarrierConnection;
    carrierCallbacks.friend_request = OnCarrierFriendRequest;
    carrierCallbacks.friend_connection = OnCarrierFriendConnection;
    carrierCallbacks.friend_message = OnCarrierFriendMessage;

    carrierOpts.udp_enabled = config->mCarrierConfig->mEnableUdp;
    carrierOpts.persistent_location = config->mUserDataDir.c_str();
    if(profile.empty() == false) {
        carrierOpts.secret_key = profile.c_str();
    }

    // set BootstrapNode
    size_t carrierNodeSize = config->mCarrierConfig->mBootstrapNodes.size();
    BootstrapNode carrierNodeArray[carrierNodeSize];
    memset (carrierNodeArray, 0, sizeof(carrierNodeArray));
    for(int idx = 0; idx < carrierNodeSize; idx++) {
        const auto& node = config->mCarrierConfig->mBootstrapNodes[idx];
        carrierNodeArray[idx].ipv4 = node.mIpv4.c_str();
        carrierNodeArray[idx].port = node.mPort.c_str();
        carrierNodeArray[idx].public_key = node.mPublicKey.c_str();
    }
    carrierOpts.bootstraps_size = carrierNodeSize;
    carrierOpts.bootstraps = carrierNodeArray;

    // set HiveBootstrapNode
    size_t hiveNodeSize = config->mCarrierConfig->mHiveNodes.size();
    HiveBootstrapNode hiveNodeArray[hiveNodeSize];
    memset (hiveNodeArray, 0, sizeof(hiveNodeArray));
    for(int idx = 0; idx < hiveNodeSize; idx++) {
        const auto& node = config->mCarrierConfig->mHiveNodes[idx];
        hiveNodeArray[idx].ipv4 = node.mIpv4.c_str();
        hiveNodeArray[idx].port = node.mPort.c_str();
    }
    //carrierOpts.hive_bootstraps_size = hiveNodeSize;
    //carrierOpts.hive_bootstraps = hiveNodeArray;

    ela_log_init(static_cast<ElaLogLevel>(config->mCarrierConfig->mLogLevel), nullptr, nullptr);

    auto creater = [&]() -> ElaCarrier* {
        auto ptr = ela_new(&carrierOpts, &carrierCallbacks, this);
        return ptr;
    };
    auto deleter = [=](ElaCarrier* ptr) -> void {
        if(ptr != nullptr) {
            ela_session_cleanup(ptr);
            ela_kill(ptr);
        }
    };
    mCarrier = std::unique_ptr<ElaCarrier, std::function<void(ElaCarrier*)>>(creater(), deleter);

    if (mCarrier == nullptr) {
        Log::E(Log::TAG, "Failed to new carrier!");
        return ErrCode::ChannelFailedCarrier;
    }

    std::string address;
    int ret = ChannelImplCarrier::getAddress(address);
    if(ret < 0) {
        return ret;
    }
    Log::I(Log::TAG, "ChannelImplCarrier::preset() Success new carrier on address: %s.", address.c_str());

    return 0;
}

int ChannelImplCarrier::open()
{
    if(mTaskThread == nullptr) {
        mTaskThread = std::make_unique<ThreadPool>();
    }
    mTaskThread->post(std::bind(&ChannelImplCarrier::runCarrier, this));

    return 0;
}

int ChannelImplCarrier::close()
{
    if (mCarrier == nullptr) {
        ela_session_cleanup(mCarrier.get());
        ela_kill(mCarrier.get());
    }
    mCarrier = nullptr;

    return 0;
}

int ChannelImplCarrier::getAddress(std::string& address)
{
    char addr[ELA_MAX_ADDRESS_LEN + 1] = {0};
    auto ret = ela_get_address(mCarrier.get(), addr, sizeof(addr));
    if(ret == nullptr) {
        int err = ela_get_error();
        char strerr_buf[512] = {0};
        ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
        Log::E(Log::TAG, "Failed to get address! ret=%s(0x%x)", strerr_buf, err);
        return ErrCode::ChannelFailedCarrier;
    }

    address = addr;
    return 0;
}

bool ChannelImplCarrier::isReady()
{
    return (mChannelStatus == ChannelListener::ChannelStatus::Online);
}

int ChannelImplCarrier::requestFriend(const std::string& friendCode,
                                      const std::string& summary,
                                      bool remoteRequest)
{
    std::function<bool(const char*)> validCheckFun = (remoteRequest ? ela_address_is_valid : ela_id_is_valid);
    bool valid = validCheckFun(friendCode.c_str());
    if(valid != true) {
        return ErrCode::InvalidArgument;
    }

    std::string selfAddr, selfUsrId;
    getAddress(selfAddr);
    GetCarrierUsrIdByAddress(selfAddr, selfUsrId);
    if(friendCode == selfAddr
    || friendCode == selfUsrId) {
        return ErrCode::ChannelFailedFriendSelf;
    }

    int ret = ErrCode::UnknownError;
    if(remoteRequest == true) {
        const char* hello = (summary.empty() ? "" : summary.c_str());
        Log::I(Log::TAG, "ChannelImplCarrier::requestFriend() summary=%s", hello);
        ret = ela_add_friend(mCarrier.get(), friendCode.c_str(), hello);
    } else {
        ret = ela_accept_friend(mCarrier.get(), friendCode.c_str());
    }
    if(ret != 0) {
        int err = ela_get_error();
        if(err == ELA_GENERAL_ERROR(ELAERR_ALREADY_EXIST)) {
            return ErrCode::ChannelFailedFriendExists;
        }

        char strerr_buf[512] = {0};
        ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
        Log::E(Log::TAG, "Failed to add friend! ret=%s(0x%x)", strerr_buf, err);
        return ErrCode::ChannelFailedCarrier;
    }

    return 0;
}

int ChannelImplCarrier::sendMessage(const std::string& friendCode,
                                    std::vector<uint8_t> msgContent)
{
    int pkgCount = msgContent.size() / MaxPkgSize + 1;
    Log::D(Log::TAG, "ChannelImplCarrier::sendMessage() size=%d count=%d", msgContent.size(), pkgCount);
    if(pkgCount > 255) {
        return ErrCode::ChannelDataTooLarge;
    }

    for(auto pkgIdx = 0; pkgIdx < pkgCount; pkgIdx++) {
        std::vector<uint8_t> data {std::begin(PkgMagic), std::end(PkgMagic)};
        data[PkgMagicDataIdx] = static_cast<uint8_t>(pkgIdx);
        data[PkgMagicDataCnt] = static_cast<uint8_t>(pkgCount);

        auto dataBegin = msgContent.begin() + pkgIdx * MaxPkgSize;
        auto dataRemains = msgContent.end() - dataBegin;
        auto dataEnd = (MaxPkgSize < dataRemains ? dataBegin + MaxPkgSize : dataBegin + dataRemains);
        data.insert(data.end(), dataBegin, dataEnd);

        int ret = ela_send_friend_message(mCarrier.get(), friendCode.c_str(), data.data(), data.size());
        if(ret != 0) {
            int err = ela_get_error();
            char strerr_buf[512] = {0};
            ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
            Log::E(Log::TAG, "Failed to send message! ret=%s(0x%x)", strerr_buf, err);
            return ErrCode::ChannelNotSendMessage;
        }
    }

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/
void ChannelImplCarrier::runCarrier()
{
    int ret = ela_run(mCarrier.get(), 500);
    if(ret < 0) {
        ela_kill(mCarrier.get());
        Log::E(Log::TAG, "Failed to run carrier!");
        return;
    }
}

void ChannelImplCarrier::OnCarrierConnection(ElaCarrier *carrier,
                                             ElaConnectionStatus status, void *context)
{
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);

    std::string carrierAddr, carrierUsrId;
    channel->getAddress(carrierAddr);
    GetCarrierUsrIdByAddress(carrierAddr, carrierUsrId);

    channel->mChannelStatus = ( status == ElaConnectionStatus_Connected
                              ? ChannelListener::ChannelStatus::Online
                              : ChannelListener::ChannelStatus::Offline);
    Log::D(Log::TAG, "OnCarrierConnection status: %d", channel->mChannelStatus);
    if(channel->mChannelListener.get() != nullptr) {
        channel->mChannelListener->onStatusChanged(carrierUsrId, channel->mChannelType, channel->mChannelStatus);
    }
}

void ChannelImplCarrier::OnCarrierFriendRequest(ElaCarrier *carrier, const char *friendid,
                                                const ElaUserInfo *info,
                                                const char *hello, void *context)
{
    Log::D(Log::TAG, "OnCarrierFriendRequest from: %s", friendid);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);

    if(channel->mChannelListener.get() != nullptr) {
        channel->mChannelListener->onFriendRequest(friendid, channel->mChannelType, hello);
    }
}

void ChannelImplCarrier::OnCarrierFriendConnection(ElaCarrier *carrier,const char *friendid,
                                                   ElaConnectionStatus status, void *context)
{
    Log::D(Log::TAG, "OnCarrierFriendConnection from: %s %d", friendid, status);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);

    if(channel->mChannelListener.get() != nullptr) {
        auto chStatus = ( status == ElaConnectionStatus_Connected
                        ? ChannelListener::ChannelStatus::Online
                        : ChannelListener::ChannelStatus::Offline);
        channel->mChannelListener->onFriendStatusChanged(friendid, channel->mChannelType, chStatus);
    }
}

void ChannelImplCarrier::OnCarrierFriendMessage(ElaCarrier *carrier, const char *from,
                                                const void *msg, size_t len, void *context)
{
    Log::D(Log::TAG, "OnCarrierFriendMessage from: %s len=%d", from, len);

    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);
    if(channel->mChannelListener.get() == nullptr) {
        Log::W(Log::TAG, "ChannelListener is not set. ignore to process");
        return;
    }

    auto data = reinterpret_cast<const uint8_t*>(msg);

    int32_t dataOffset = 0;
    bool dataComplete = true;
    bool isPkgData = true;
    for(auto idx = 0; idx < PkgMagicHeadSize; idx++) {
        if(data[idx] != PkgMagic[idx]) {
            isPkgData = false;
            break;
        }
    }
    if(isPkgData == true) {
        dataOffset = PkgMagicSize;
        dataComplete = (data[PkgMagicDataIdx] == data[PkgMagicDataCnt] - 1 ? true : false);
        Log::D(Log::TAG, "OnCarrierFriendMessage PkgMagicData Idx/Cnt=%d/%d", data[PkgMagicDataIdx], data[PkgMagicDataCnt]);
    }

    auto& dataCache = channel->mRecvDataCache[from];
    dataCache.insert(dataCache.end(), data + dataOffset, data + len);

    if(dataComplete == true) {
        channel->mChannelListener->onReceivedMessage(from, channel->mChannelType, dataCache);
        dataCache.clear();
    }
}

/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
