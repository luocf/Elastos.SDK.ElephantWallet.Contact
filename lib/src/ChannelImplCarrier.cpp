//
//  ChannelImplCarrier.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ChannelImplCarrier.hpp>

#include <Log.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ChannelImplCarrier::ChannelImplCarrier(std::weak_ptr<Config> config)
    : MessageChannelStrategy()
    , mConfig(config)
    , mCarrier()
    , mTaskThread()
{
}

ChannelImplCarrier::~ChannelImplCarrier()
{
}

int ChannelImplCarrier::open()
{
    if(mCarrier != nullptr) {
        return ErrCode::ChannelFailedMultiOpen;
    }

    auto config = mConfig.lock();

    ElaOptions carrierOpts;
    ElaCallbacks carrierCallbacks;

    memset(&carrierOpts, 0, sizeof(carrierOpts));
    memset(&carrierCallbacks, 0, sizeof(carrierCallbacks));

    carrierOpts.udp_enabled = config->mCarrierConfig->mEnableUdp;
    carrierOpts.persistent_location = config->mUserDataDir.c_str();

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
    carrierOpts.hive_bootstraps_size = hiveNodeSize;
    carrierOpts.hive_bootstraps = hiveNodeArray;

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
        return ErrCode::ChannelFailedNewCarrier;
    }

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

int ChannelImplCarrier::isReady()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int ChannelImplCarrier::sendMessage(FriendInfo friendInfo,
                                    int msgType, std::string msgContent)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}
int ChannelImplCarrier::sendMessage(FriendInfo friendInfo,
                                    int msgType, std::vector<int8_t> msgContent)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
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


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
