//
// Created by luocf on 2019-11-14.
//
#include "MicroServiceTest.hpp"
#include "P2PNetworkBridge.hpp"

namespace elastos {
    MicroServiceTest::MicroServiceTest() {
        elastos::P2PNetworkBridge& p2pNetworkBridge = elastos::P2PNetworkBridge::getInstance();
        mP2PNetworkBase = p2pNetworkBridge.GetP2PNetwork();
        mP2PNetworkBase->bindMicrService(this);
    }

    MicroServiceTest::~MicroServiceTest() {
        this->Stop();
    }

    void MicroServiceTest::Create(const std::string &pub_key, const std::string &dir) {
        mP2PNetworkBase->Create(pub_key, dir);
    }

    void MicroServiceTest::Start() {
        mP2PNetworkBase->Start();
    }

    void MicroServiceTest::Stop() {
        mP2PNetworkBase->Stop();
    }

    void MicroServiceTest::HandleMessage(const std::string &humanCode, const std::string &msg) {
        //Log::I(Log::TAG, "MicroServiceTest HandleMessage Test:%s", humanCode.c_str());
    }
}