#include "P2PNetworkSdk.hpp"
#include <MessageManager.hpp>
#include <memory>

namespace elastos {
    P2PNetworkSdk* P2PNetworkSdk::sP2PNetworkSdk = nullptr;

    P2PNetworkSdk::P2PNetworkSdk() {
        sP2PNetworkSdk = this;
    }

    P2PNetworkSdk::~P2PNetworkSdk() {
        this->Stop();
    }

    std::string P2PNetworkSdk::GetPubKey() {
        return mPubKey;
    }

    void P2PNetworkSdk::Create(const std::string &pub_key, const std::string &dir) {
        mPubKey = pub_key;
        mDir = dir;
        elastos::sdk::ElephantContact::Factory::SetLogLevel(4);
        int ret = elastos::sdk::ElephantContact::Factory::SetLocalDataDir(mDir.c_str());
        if (ret < 0) {
            throw std::runtime_error(
                    std::string("Failed to set contact local dir! ret=") + std::to_string(ret));
        }

        mElephantContact = elastos::sdk::ElephantContact::Factory::Create();
        class LocalListener : public elastos::sdk::ElephantContact::Listener {
        public:
            LocalListener() {}

            ~LocalListener() {}

            std::shared_ptr <std::vector<uint8_t>>
            onAcquire(elastos::sdk::ElephantContact::Listener::AcquireType type,
                      const std::string &pubKey,
                      const std::vector <uint8_t> &data) override {
                std::shared_ptr <std::vector<uint8_t>> result_data;
                switch (type) {
                    case elastos::sdk::ElephantContact::Listener::AcquireType::PublicKey: {
                        //std::string publicKey = "02bc11aa5c35acda6f6f219b94742dd9a93c1d11c579f98f7e3da05ad910a48306";
                        //const char* privateKey = "543c241f89bebb660157bcd12d7ab67cf69f3158240a808b22eb98447bad205d";
                        std::string publicKey = sP2PNetworkSdk->GetPubKey();
                        result_data = std::make_shared < std::vector <
                                      uint8_t >> (publicKey.data(), publicKey.data() +
                                                                    publicKey.size());
                        //data = std::make_shared<std::vector<uint8_t>>(publicKey.begin(), publicKey.end());
                        break;
                    }

                    case elastos::sdk::ElephantContact::Listener::AcquireType::EncryptData:
                    case elastos::sdk::ElephantContact::Listener::AcquireType::DecryptData:
                    case elastos::sdk::ElephantContact::Listener::AcquireType::DidPropAppId:
                    case elastos::sdk::ElephantContact::Listener::AcquireType::DidAgentAuthHeader:
                    case elastos::sdk::ElephantContact::Listener::AcquireType::SignData: {
                        result_data = std::make_shared < std::vector < uint8_t >> (data);
                        break;
                    }

                }
                return result_data;
            }

            void onEvent(elastos::sdk::ElephantContact::Listener::EventType type,
                         const std::string &humanCode, ContactChannel channelType,
                         const std::vector <uint8_t> &data) override {
                //TODO,数据转换
                switch (type) {
                    case elastos::sdk::ElephantContact::Listener::EventType::StatusChanged:
                        sP2PNetworkSdk->onReceivedMessage(humanCode, "");
                        break;
                    case elastos::sdk::ElephantContact::Listener::EventType::FriendReuqest:
                        sP2PNetworkSdk->AcceptFriend(humanCode.c_str());
                        break;
                    case elastos::sdk::ElephantContact::Listener::EventType::HumanInfoChanged:
                        sP2PNetworkSdk->onReceivedMessage(humanCode, "");
                        break;
                }
            }

            void onReceivedMessage(const std::string &humanCode,
                                   elastos::sdk::ElephantContact::Listener::ContactChannel channelType,
                                   std::shared_ptr <elastos::MessageManager::MessageInfo> msgInfo) override {
                //TODO,数据转换
                sP2PNetworkSdk->onReceivedMessage(humanCode, "");
            }

            void onError(int errCode, const std::string &errStr, const std::string &ext) override {
                //打印 error log
            }
        };
        auto msgListener = std::make_shared<LocalListener>();
        mElephantContact->setListener(msgListener.get());
    };

    void P2PNetworkSdk::Start() {
        if (mElephantContact.get() != nullptr) {
            int ret = mElephantContact->start();
            if (ret < 0) {
                throw std::runtime_error(
                        std::string("Failed to start contact! ret=") + std::to_string(ret));
            }
        }
    };

    void P2PNetworkSdk::Stop() {
        if (mElephantContact.get() != nullptr) {
            int ret = mElephantContact->stop();
            if (ret < 0) {
                throw std::runtime_error(
                        std::string("Failed to stop contact! ret=") + std::to_string(ret));
            }
        }
    };

    int P2PNetworkSdk::GetHumanInfo(const char *humanCode, std::stringstream *info) {
        if (mElephantContact.get() != nullptr) {
            return mElephantContact->getHumanInfo(humanCode, info);
        }
        return -1;
    };

    int P2PNetworkSdk::GetHumanStatus(const char *humanCode) {
        if (mElephantContact.get() != nullptr) {
            return mElephantContact->getHumanStatus(humanCode);
        }
        return -1;
    };

    void P2PNetworkSdk::AcceptFriend(const char *friendCode) {
        if (mElephantContact.get() != nullptr) {
            mElephantContact->acceptFriend(friendCode);
        }
    }
}
