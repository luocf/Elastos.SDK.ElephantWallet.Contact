//
//  ChannelImplCarrier.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ChannelImplCarrier.hpp>

#include <ela_session.h>
#include <cstring>
#include <Log.hpp>
#include <SafePtr.hpp>
#include <Platform.hpp>
//#include <jni.h>

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
                                       std::shared_ptr<ChannelListener> chListener,
                                       std::shared_ptr<ChannelDataListener> dataListener,
                                       std::weak_ptr<Config> config)
    : MessageChannelStrategy(chType, chListener, dataListener)
    , mConfig(config)
    , mCarrier()
    , mTaskThread()
    , mChannelStatus(ChannelListener::ChannelStatus::Pending)
    , mRecvDataCache()
    , mDataTransMutex()
    , mDataTransThread()
    , mCarrierFileTrans()
    , mDataTransType(Invalid)
    , mDataTransStatus(ChannelDataListener::Status::Unknown)
    , mDataId()
    , mDataRecvOffset(0)
{
#if defined(__ANDROID__)
    Platform::CallOnload(ela_session_jni_onload);
#endif // defined(__ANDROID__)
}

ChannelImplCarrier::~ChannelImplCarrier()
{
    close();
}

int ChannelImplCarrier::preset(const std::string& profile)
{
    if(mCarrier != nullptr) {
        return ErrCode::ChannelFailedMultiOpen;
    }

    int ret = initCarrier();
    if(ret < 0) {
        return ret;
    }

    ret = ela_set_self_nospam(mCarrier.get(), 0);
    if(ret != 0) {
        int err = ela_get_error();
        char strerr_buf[512] = {0};
        ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
        Log::E(Log::TAG, "Failed to set carrier nospam! ret=%s(0x%x)", strerr_buf, err);
        return ErrCode::ChannelFailedCarrier;
    }


    std::string address;
    ret = ChannelImplCarrier::getAddress(address);
    CHECK_ERROR(ret)
    Log::I(Log::TAG, "ChannelImplCarrier::preset() Success preset carrier on address: %s.", address.c_str());



    return 0;
}

int ChannelImplCarrier::open()
{
    if(mTaskThread == nullptr) {
        mTaskThread = std::make_unique<ThreadPool>("carrier-channel");
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
                                      bool remoteRequest,
                                      bool forceRequest)
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
        std::string usrId;
        ret = GetCarrierUsrIdByAddress(friendCode, usrId);
        CHECK_ERROR(ret)

        const char* hello = (summary.empty() ? " " : summary.c_str());
        bool isAdded = ela_is_friend(mCarrier.get(), usrId.c_str());
        if(isAdded == true) {
            if(forceRequest == false) {
                Log::I(Log::TAG, "ChannelImplCarrier::requestFriend() Friend is already exists. friendCode=%s summary=%s", friendCode.c_str(), hello);
                return ErrCode::ChannelFailedFriendExists;
            }
            ela_remove_friend(mCarrier.get(), usrId.c_str());
        }

        Log::I(Log::TAG, "ChannelImplCarrier::requestFriend() friendCode=%s summary=%s", friendCode.c_str(), hello);
        ret = ela_add_friend(mCarrier.get(), friendCode.c_str(), hello);
    } else {
        Log::I(Log::TAG, "ChannelImplCarrier::requestFriend() friendCode=%s", friendCode.c_str());
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

int ChannelImplCarrier::removeFriend(const std::string& friendCode)
{
    std::string usrId;
    bool valid = IsValidCarrierUsrId(friendCode);
    if(valid == true) {
        usrId = friendCode;
    } else {
        bool valid = IsValidCarrierAddress(friendCode);
        if (valid == true) {
            GetCarrierUsrIdByAddress(friendCode, usrId);
        }
    }
    if(usrId.empty() == true) {
        return ErrCode::InvalidArgument;
    }

    bool isFriend = ela_is_friend(mCarrier.get(), usrId.c_str());
    if(isFriend == false) {
        Log::W(Log::TAG, "Ignore to remove a friend: %s, it's not exists.", usrId.c_str());
        return 0;
    }

    int ret = ela_remove_friend(mCarrier.get(), usrId.c_str());
    if(ret != 0) {
        int err = ela_get_error();
        if(err == ELA_GENERAL_ERROR(ELAERR_ALREADY_EXIST)) {
            return ErrCode::ChannelFailedFriendExists;
        }

        char strerr_buf[512] = {0};
        ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
        Log::E(Log::TAG, "Failed to remove friend! ret=%s(0x%x)", strerr_buf, err);
        return ErrCode::ChannelFailedCarrier;
    }

    return 0;
}

int ChannelImplCarrier::sendMessage(const std::string& friendCode,
                                    std::vector<uint8_t> msgContent)
{
    uint64_t pkgCount = msgContent.size() / MaxPkgSize + 1;
    Log::D(Log::TAG, "ChannelImplCarrier::sendMessage() size=%d count=%d", msgContent.size(), pkgCount);
    if(pkgCount > MaxPkgCount) {
        return ErrCode::ChannelDataTooLarge;
    }

    for(uint64_t pkgIdx = 0; pkgIdx < pkgCount; pkgIdx++) {
        std::vector<uint8_t> data {std::begin(PkgMagic), std::end(PkgMagic)};
        uint16_t pkgVal = static_cast<uint16_t>(pkgIdx);
        data[PkgMagicDataIdx] = (pkgVal >> 8 ) & 0xFF;
        data[PkgMagicDataIdx + 1] = pkgVal & 0xFF;
        pkgVal = static_cast<uint16_t>(pkgCount);
        data[PkgMagicDataCnt] = (pkgVal >> 8 ) & 0xFF;
        data[PkgMagicDataCnt + 1] = pkgVal & 0xFF;

        auto dataBegin = msgContent.begin() + pkgIdx * MaxPkgSize;
        auto dataRemains = msgContent.end() - dataBegin;
        auto dataEnd = (MaxPkgSize < dataRemains ? dataBegin + MaxPkgSize : dataBegin + dataRemains);
        data.insert(data.end(), dataBegin, dataEnd);

        bool offlineMsg;
        int ret = ela_send_friend_message(mCarrier.get(), friendCode.c_str(), data.data(), data.size(), &offlineMsg);
        if(ret != 0) {
            int err = ela_get_error();
            char strerr_buf[512] = {0};
            ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
            Log::E(Log::TAG, "Failed to send message! ret=%s(0x%x)", strerr_buf, err);
            return ErrCode::ChannelNotSendMessage;
        }
        Log::D(Log::TAG, "ChannelImplCarrier::sendMessage PkgMagicData Idx/Cnt=%05lld[%02x,%02x]/%05lld[%02x,%02x]",
                         pkgIdx, data[PkgMagicDataIdx], data[PkgMagicDataIdx + 1], pkgCount, data[PkgMagicDataCnt], data[PkgMagicDataCnt + 1]);
    }

    return 0;
}

int ChannelImplCarrier::sendData(const std::string& friendCode,
                                 const std::string& dataId)
{
    if(dataId.empty() == true
    || dataId.length() > ELA_MAX_FILE_NAME_LEN) {
        return ErrCode::ChannelInvalidDataId;
    }

    mDataId = dataId;

    int ret = makeCarrierFileTrans(friendCode.c_str(), true);
    CHECK_ERROR(ret);

    std::unique_lock<std::recursive_mutex> lock(*mDataTransMutex);
    ret = ela_filetransfer_connect(mCarrierFileTrans.get());
    if (ret < 0) {
        Log::E(Log::TAG, "Failed to connect filetransfer!");
        return ErrCode::ChannelFailedCarrier;
    }

    return 0;
}

int ChannelImplCarrier::cancelSendData(const std::string& friendCode,
                                       const std::string& dataId)
{
    Log::I(Log::TAG, "%s friendCode=%s dataId=%s", __PRETTY_FUNCTION__, friendCode.c_str(), dataId.c_str());
//    std::unique_lock<std::recursive_mutex> lock(*mDataTransMutex);
    mCarrierFileTrans.reset();

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/
int ChannelImplCarrier::initCarrier()
{
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
//    Log::W(Log::TAG, "========= Recover carrier by secret key: %s", profile.c_str());
//    if(profile.empty() == false) {
//        carrierOpts.secret_key = profile.c_str();
//    }

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

    carrierOpts.log_level = static_cast<ElaLogLevel>(config->mCarrierConfig->mLogLevel);

    auto creater = [&]() -> ElaCarrier* {
        auto ptr = ela_new(&carrierOpts, &carrierCallbacks, this);
        return ptr;
    };
    auto deleter = [=](ElaCarrier* ptr) -> void {
        if(ptr != nullptr) {
            ela_filetransfer_cleanup(ptr);
            ela_session_cleanup(ptr);
            ela_kill(ptr);
        }
    };
    mCarrier = std::unique_ptr<ElaCarrier, std::function<void(ElaCarrier*)>>(creater(), deleter);
    if (mCarrier == nullptr) {
        Log::E(Log::TAG, "Failed to new carrier!");
        return ErrCode::ChannelFailedCarrier;
    }

    int ret = ela_filetransfer_init(mCarrier.get(), DataRecvListener::OnConnect, this);
    if (ret < 0) {
        Log::E(Log::TAG, "Failed to init filetransfer!");
        return ErrCode::ChannelFailedCarrier;
    }

    return 0;
}

int ChannelImplCarrier::makeCarrierFileTrans(const char* friendCode, bool sendOrRecv)
{
    if(mDataTransMutex.get() == nullptr) {
        mDataTransMutex = std::make_shared<std::recursive_mutex>();
    }

    std::unique_lock<std::recursive_mutex> lock(*mDataTransMutex);
    if(mDataTransThread.get() == nullptr) {
        mDataTransThread = std::make_unique<ThreadPool>("carrier-channel-datatrans");
    }

    if(mCarrierFileTrans.get() != nullptr) {
        return ErrCode::ChannelFileTransBusy;
    }

    ElaFileTransferCallbacks ftCallbacks = {};
    ftCallbacks.state_changed = DataTransListener::OnStateChanged;
//    ftCallbacks.cancel        = DataTransListener::OnCancel;
    ftCallbacks.pull          = DataSendListener::OnPull;
    ftCallbacks.file          = DataRecvListener::OnFile;
    ftCallbacks.data          = DataRecvListener::OnData;

    auto creater = [&]() -> ElaFileTransfer* {
        Log::I(Log::TAG, "%s ela_filetransfer_new()", __PRETTY_FUNCTION__);
//        std::unique_lock<std::recursive_mutex> lock(*mDataTransMutex);
        auto ptr = ela_filetransfer_new(mCarrier.get(), friendCode, nullptr,
                                        &ftCallbacks, this);
        setDataTransStatus(ChannelDataListener::Status::Initialized);
        return ptr;
    };
    auto deleter = [=](ElaFileTransfer* ptr) -> void {
        if(ptr == nullptr) {
            return;
        }

        Log::I(Log::TAG, "%s ela_filetransfer_close()", __PRETTY_FUNCTION__);
//            std::unique_lock<std::recursive_mutex> lock(*mDataTransMutex);
        ela_filetransfer_close(ptr);
        setDataTransStatus(ChannelDataListener::Status::Destroyed);
    };
    mCarrierFileTrans = std::unique_ptr<ElaFileTransfer, std::function<void(ElaFileTransfer*)>>(creater(), deleter);
    if (mCarrierFileTrans.get() == nullptr) {
        int err = ela_get_error();
        char strerr_buf[512] = {0};
        ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
        Log::E(Log::TAG, "Failed to make file transfer! ret=%s(0x%x)", strerr_buf, err);
        return ErrCode::ChannelFailedCarrier;
    }
    mDataTransType = (sendOrRecv ? Sender : Receiver);
    mDataRecvOffset = 0;

    mFriendId = friendCode;
    Log::I(Log::TAG, "Success to make file transfer");

    return 0;
}

void ChannelImplCarrier::runCarrier()
{
    int ret = ela_run(mCarrier.get(), 500);
    if(ret < 0) {
        ela_kill(mCarrier.get());
        Log::E(Log::TAG, "Failed to run carrier!");
        return;
    }
}

void ChannelImplCarrier::runSendData(const std::string fileid, uint64_t offset)
{
    mDataRecvOffset = offset;
    std::vector<uint8_t> data;
    int closeReason;

    setDataTransStatus(ChannelDataListener::Status::Transmitting);
    for(;;) {
        std::unique_lock<std::recursive_mutex> lock(*mDataTransMutex);
        Log::I(Log::TAG, "%s %d", __PRETTY_FUNCTION__, __LINE__);
        if(mCarrierFileTrans.get() == nullptr) { // data transfer closed.
            Log::W(Log::TAG, "Failed to send data. user closed.");
            break;
        }

        data.clear();
        int ret = mChannelDataListener->onReadData(mFriendId, mChannelType,
                                               mDataId, mDataRecvOffset,
                                               data);
        if(ret > 0) {
            ret = ela_filetransfer_send(mCarrierFileTrans.get(), fileid.c_str(), data.data(), data.size());
            if(ret > 0) {
                mDataRecvOffset += ret;
                continue;
            }

            int err = ela_get_error();
            char strerr_buf[512] = {0};
            ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
            Log::E(Log::TAG, "Failed to send data ret=%s(0x%x)", strerr_buf, err);
            closeReason = err;
            // close transfer
        } else if(ret == 0) { // finished
            ret = ela_filetransfer_send(mCarrierFileTrans.get(), fileid.c_str(), nullptr, 0);
            if(ret < 0) {
                int err = ela_get_error();
                char strerr_buf[512] = {0};
                ela_get_strerror(err, strerr_buf, sizeof(strerr_buf));
                Log::E(Log::TAG, "Failed to cancel send data err=%s(0x%x)", strerr_buf, err);
            }
            Log::I(Log::TAG, "Finish to send data.");
            closeReason = 0;
        } else {
            closeReason = ret;
        }

        break;
    }

    Log::I(Log::TAG, "%s end.", __PRETTY_FUNCTION__);

    std::unique_lock<std::recursive_mutex> lock(*mDataTransMutex);
//    mCarrierFileTrans.reset();
}

void ChannelImplCarrier::setDataTransStatus(ChannelDataListener::Status status)
{
    if(mDataTransStatus != status) {
        mChannelDataListener->onNotify(mFriendId, mChannelType, mDataId, status);
    }

    mDataTransStatus = status;
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
    Log::D(Log::TAG, "ChannelImplCarrier::OnCarrierConnection status: %d", channel->mChannelStatus);
    if(channel->mChannelListener.get() != nullptr) {
        channel->mChannelListener->onStatusChanged(carrierUsrId, channel->mChannelType, channel->mChannelStatus);
    }
}

void ChannelImplCarrier::OnCarrierFriendRequest(ElaCarrier *carrier, const char *friendid,
                                                const ElaUserInfo *info,
                                                const char *hello, void *context)
{
    Log::D(Log::TAG, "ChannelImplCarrier::OnCarrierFriendRequest from: %s", friendid);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);

    if(channel->mChannelListener.get() != nullptr) {
        channel->mChannelListener->onFriendRequest(friendid, channel->mChannelType, hello);
    }
}

void ChannelImplCarrier::OnCarrierFriendConnection(ElaCarrier *carrier,const char *friendid,
                                                   ElaConnectionStatus status, void *context)
{
    Log::D(Log::TAG, "=-=-=-=-= ChannelImplCarrier::OnCarrierFriendConnection from: %s %d", friendid, status);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);

    if(channel->mChannelListener.get() != nullptr) {
        auto chStatus = ( status == ElaConnectionStatus_Connected
                        ? ChannelListener::ChannelStatus::Online
                        : ChannelListener::ChannelStatus::Offline);
        channel->mChannelListener->onFriendStatusChanged(friendid, channel->mChannelType, chStatus);
    }
}

void ChannelImplCarrier::OnCarrierFriendMessage(ElaCarrier *carrier, const char *from,
                                                const void *msg, size_t len,
                                                bool offline, void *context)
{
    Log::D(Log::TAG, "ChannelImplCarrier::OnCarrierFriendMessage from: %s len=%d", from, len);

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
        dataOffset = PkgMagicSize;
    }

    auto dataSection = std::vector<uint8_t>(data + dataOffset, data + len);
    if(isPkgData == true) {
        uint64_t pkgIdx = data[PkgMagicDataIdx];
        pkgIdx = (pkgIdx << 8) + data[PkgMagicDataIdx + 1];
        uint64_t pkgCount = data[PkgMagicDataCnt];
        pkgCount = (pkgCount << 8) + data[PkgMagicDataCnt + 1];

//        dataComplete = (pkgIdx == pkgCount - 1 ? true : false);
        Log::D(Log::TAG, "ChannelImplCarrier::OnCarrierFriendMessage PkgMagicData Idx/Cnt=%05lld[%02x,%02x]/%05lld[%02x,%02x]",
               pkgIdx, data[PkgMagicDataIdx], data[PkgMagicDataIdx + 1], pkgCount, data[PkgMagicDataCnt], data[PkgMagicDataCnt + 1]);

        auto& dataCache = channel->mRecvDataCache[from];
        dataCache[pkgIdx] = std::move(dataSection);

        if(dataCache.size() == pkgCount) {
            std::vector<uint8_t> dataPkg;
            for(int idx = 0; idx < dataCache.size(); idx++) {
                dataPkg.insert(dataPkg.end(), dataCache[idx].begin(), dataCache[idx].end());
            }

            channel->mChannelListener->onReceivedMessage(from, channel->mChannelType, dataPkg);
            dataCache.clear();
        }
    } else {
        channel->mChannelListener->onReceivedMessage(from, channel->mChannelType, dataSection);
    }

}

void ChannelImplCarrier::DataRecvListener::OnConnect(ElaCarrier *carrier,
                                                     const char *from,
                                                     const ElaFileTransferInfo *fileinfo,
                                                     void *context)
{
    Log::I(Log::TAG, "%s from=%s fileinfo=%p", __PRETTY_FUNCTION__, from, fileinfo);
    if(fileinfo != nullptr) {
        Log::I(Log::TAG, "filename=%s", fileinfo->filename);
    }
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);

    int ret = channel->makeCarrierFileTrans(from, false);
    CHECK_RETVAL(ret);

    std::unique_lock<std::recursive_mutex> lock(*channel->mDataTransMutex);
    ret = ela_filetransfer_accept_connect(channel->mCarrierFileTrans.get());
    if (ret < 0) {
        Log::E(Log::TAG, "Failed to accept filetransfer!");
        return;
    }
}

void ChannelImplCarrier::DataTransListener::OnStateChanged(ElaFileTransfer *filetransfer,
                                                           FileTransferConnection state,
                                                           void *context)
{
    Log::I(Log::TAG, "%s state=%d", __PRETTY_FUNCTION__, state);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);
    std::unique_lock<std::recursive_mutex> lock(*channel->mDataTransMutex);

    ChannelDataListener::Status dataStatus = ChannelDataListener::Status::Unknown;

    switch (state) {
        case FileTransferConnection_connecting:
            dataStatus = ChannelDataListener::Status::Connecting;
            break;
        case FileTransferConnection_connected:
            dataStatus = ChannelDataListener::Status::Connected;
            if(channel->mDataTransType == Sender) { // sender
                ElaFileTransferInfo fileInfo = {};

                strncpy(fileInfo.filename, "carrier bug workaround", ELA_MAX_FILE_NAME_LEN);
                fileInfo.size = -1; // not used
                ela_filetransfer_add(filetransfer, &fileInfo);

                strcpy(fileInfo.filename, channel->mDataId.c_str());
                fileInfo.size = -1; // not used
                int ret = ela_filetransfer_add(filetransfer, &fileInfo);
                Log::I(Log::TAG, "%s add ret=%d", __PRETTY_FUNCTION__, ret);
                CHECK_RETVAL(ret);
            }
            break;
        case FileTransferConnection_closed:
            dataStatus = ChannelDataListener::Status::Closed;
            channel->mDataTransThread->post([=]() {
                channel->mCarrierFileTrans.reset();
            });
            break;
        case FileTransferConnection_failed:
            dataStatus = ChannelDataListener::Status::Failed;
            channel->mDataTransThread->post([=]() {
                channel->mCarrierFileTrans.reset();
            });
            break;
        default:
            break;
    }

    channel->setDataTransStatus(dataStatus);

}

void ChannelImplCarrier::DataSendListener::OnPull(ElaFileTransfer *filetransfer,
                                                  const char *fileid,
                                                  uint64_t offset,
                                                  void *context)
{
    Log::I(Log::TAG, "%s fileid=%s size=%llu", __PRETTY_FUNCTION__, fileid, offset);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);
    std::unique_lock<std::recursive_mutex> lock(*channel->mDataTransMutex);

    if(channel->mDataTransThread == nullptr) {
        channel->mDataTransThread = std::make_unique<ThreadPool>("carrier-channel-datasend");
    }
    channel->mDataTransThread->post(std::bind(&ChannelImplCarrier::runSendData, channel, std::string(fileid), offset));
}

void ChannelImplCarrier::DataRecvListener::OnFile(ElaFileTransfer *filetransfer,
                                                  const char *fileid,
                                                  const char *filename, uint64_t size,
                                                  void *context)
{
    Log::I(Log::TAG, "%s fileid=%s name=%s size=%llu", __PRETTY_FUNCTION__, fileid, filename, size);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);
    std::unique_lock<std::recursive_mutex> lock(*channel->mDataTransMutex);

    channel->mDataId = filename;

    int ret = ela_filetransfer_pull(filetransfer, fileid, 0);
    CHECK_RETVAL(ret);

//    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

bool ChannelImplCarrier::DataRecvListener::OnData(ElaFileTransfer *filetransfer,
                                                  const char *fileid,
                                                  const uint8_t *data, size_t length,
                                                  void *context) {
    Log::I(Log::TAG, "%s fileid=%s data=%p size=%d", __PRETTY_FUNCTION__, fileid, data, length);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);
    std::unique_lock<std::recursive_mutex> lock(*channel->mDataTransMutex);

    channel->setDataTransStatus(ChannelDataListener::Status::Transmitting);

    std::vector<uint8_t> dataRecv;
    if (length > 0) {
        dataRecv = std::vector<uint8_t>(data, data + length);
    }

    int ret = channel->mChannelDataListener->onWriteData(channel->mFriendId, channel->mChannelType,
                                                     channel->mDataId, channel->mDataRecvOffset,
                                                     dataRecv);
    if(ret < 0) {
        return false;
    }

    channel->mDataRecvOffset += length;

    Log::I(Log::TAG, "%s fileid=%s end offset=%llu", __PRETTY_FUNCTION__, fileid, channel->mDataRecvOffset);

    if(length == 0) {
        channel->mCarrierFileTrans.reset();
    }

    return true;
}

void ChannelImplCarrier::DataTransListener::OnCancel(ElaFileTransfer *filetransfer,
                                                     const char *fileid,
                                                     int status, const char *reason,
                                                     void *context)
{
    Log::I(Log::TAG, "%s data=%s status=%d", __PRETTY_FUNCTION__, fileid, status);
    auto channel = reinterpret_cast<ChannelImplCarrier*>(context);
    std::unique_lock<std::recursive_mutex> lock(*channel->mDataTransMutex);

    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
