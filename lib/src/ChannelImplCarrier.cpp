//
//  ChannelImplCarrier.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ChannelImplCarrier.hpp>

#include "misc/Log.hpp"

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
{
}

ChannelImplCarrier::~ChannelImplCarrier()
{
}

int ChannelImplCarrier::open()
{
    auto config = mConfig.lock();

    ElaOptions carrierOpts;
    ElaCallbacks carrierCallbacks;

    memset(&carrierOpts, 0, sizeof(carrierOpts));
    memset(&carrierCallbacks, 0, sizeof(carrierCallbacks));

    size_t carrierNodeSize = config->mCarrierConfig->mBootstrapNodes.size();
    BootstrapNode carrierNodeArray[carrierNodeSize];
    memset (carrierNodeArray, 0, sizeof(carrierNodeArray));

    for(int idx = 0; idx < carrierNodeSize; idx++) {
        const auto& node = config->mCarrierConfig->mBootstrapNodes[idx];
        carrierNodeArray[idx].ipv4 = node.mIpv4.c_str();
        carrierNodeArray[idx].port = node.mPort.c_str();
        carrierNodeArray[idx].public_key = node.mPublicKey.c_str();
    }

    carrierOpts.udp_enabled = config->mCarrierConfig->mEnableUdp;
    carrierOpts.persistent_location = config->mUserDataDir.c_str();
    carrierOpts.bootstraps_size = carrierNodeSize;
    carrierOpts.bootstraps = carrierNodeArray;

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

    int ret = ela_run(mCarrier.get(), 500);
    if(ret < 0) {
        Log::E(Log::TAG, "Failed to run carrier!");
        return ErrCode::ChannelFailedRunCarrier;
    }

    return 0;
}

int ChannelImplCarrier::clone()
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


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
