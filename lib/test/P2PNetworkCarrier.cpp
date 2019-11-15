#include <memory>
#include "P2PNetworkCarrier.hpp"
namespace elastos {
    P2PNetworkCarrier* P2PNetworkCarrier::sP2PNetworkCarrier = nullptr;
    P2PNetworkCarrier::P2PNetworkCarrier() {
        sP2PNetworkCarrier = this;
    }

    P2PNetworkCarrier::~P2PNetworkCarrier() {
        this->Stop();
    }

    std::string P2PNetworkCarrier::GetPubKey() {
        return mPubKey;
    }

    void P2PNetworkCarrier::Create(const std::string &pub_key, const std::string &dir) {
        mPubKey = pub_key;
        mDir = dir;
    };

    void P2PNetworkCarrier::Start() {
    };

    void P2PNetworkCarrier::Stop() {
    };
    int P2PNetworkCarrier::GetHumanInfo(const char *humanCode, std::stringstream *info) {
        return -1;
    };

    int P2PNetworkCarrier::GetHumanStatus(const char *humanCode) {
        return -1;
    };
}
