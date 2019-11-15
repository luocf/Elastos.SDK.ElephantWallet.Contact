#include "P2PNetworkBase.hpp"

namespace elastos {
    P2PNetworkBase::P2PNetworkBase() {
    }

    P2PNetworkBase::~P2PNetworkBase() {
    }

    void P2PNetworkBase::bindMicrService(MicroService* microService) {
        mMicroService = microService;
    }

    void P2PNetworkBase::onReceivedMessage(const std::string &humanCode,
                                           const std::string &data) {
        mMicroService->HandleMessage(humanCode, data);
    }
}

