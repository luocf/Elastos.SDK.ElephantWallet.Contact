//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactListener.hpp>

#include <ContactListener.proxy.hpp>
#include "Log.hpp"

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
std::recursive_mutex ContactListener::sContactListenerMutex {};
ContactListener* ContactListener::sContactListenerInstance = nullptr;

/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ContactListener::ContactListener()
        : mSecurityListener()
        , mMessageListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(sContactListenerMutex);
    sContactListenerInstance = this;
}
ContactListener::~ContactListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
    std::lock_guard<std::recursive_mutex> lock(sContactListenerMutex);
    if(sContactListenerInstance == this) {
        sContactListenerInstance = nullptr;
    }
}

std::shared_ptr<elastos::SecurityManager::SecurityListener> ContactListener::getSecurityListener()
{
    return mSecurityListener;
}
std::shared_ptr<elastos::MessageManager::MessageListener> ContactListener::getMessageListener()
{
    return mMessageListener;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
std::shared_ptr<elastos::SecurityManager::SecurityListener> ContactListener::makeSecurityListener()
{
    class SecurityListener final : public elastos::SecurityManager::SecurityListener {
    public:
        explicit SecurityListener() = default;
        virtual ~SecurityListener() = default;

        std::string onRequestPublicKey() override {
            std::lock_guard<std::recursive_mutex> lock(sContactListenerMutex);
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            if(sContactListenerInstance == nullptr) {
                Log::W(Log::TAG, "ContactListener has been destroyed.");
                return "";
            }

            int type = CallbackType::PackRequest(CallbackType::PublicKey);
            auto ret = sContactListenerInstance->onCallback(type, nullptr);
            if(ret.get() == nullptr) {
                return "";
            }

            std::string pubKey(reinterpret_cast<char*>(ret->data()));
            return pubKey;
        }

        std::vector<uint8_t> onEncryptData(const std::string& pubKey, const std::vector<uint8_t>& src) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }
        std::vector<uint8_t> onDecryptData(const std::vector<uint8_t>& src) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        std::string onRequestDidPropAppId() override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        std::string onRequestDidAgentAuthHeader() override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        std::vector<uint8_t> onSignData(const std::vector<uint8_t>& originData) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }
    };

    return std::make_shared<SecurityListener>();
}

std::shared_ptr<elastos::MessageManager::MessageListener> ContactListener::makeMessageListener()
{
    class MessageListener final : public elastos::MessageManager::MessageListener {
    public:
        explicit MessageListener() = default;
        virtual ~MessageListener() = default;

        virtual void onStatusChanged(std::shared_ptr<elastos::UserInfo> userInfo,
                                     elastos::MessageManager::ChannelType channelType,
                                     elastos::UserInfo::Status status) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        virtual void onReceivedMessage(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                       elastos::MessageManager::ChannelType channelType,
                                       const std::shared_ptr<elastos::MessageManager::MessageInfo> msgInfo) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        virtual void onSentMessage(int msgIndex, int errCode) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        virtual void onFriendRequest(std::shared_ptr<elastos::FriendInfo> friendInfo,
                                     elastos::MessageManager::ChannelType channelType,
                                     const std::string& summary) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        virtual void onFriendStatusChanged(std::shared_ptr<elastos::FriendInfo> friendInfo,
                                           elastos::MessageManager::ChannelType channelType,
                                           elastos::FriendInfo::Status status) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }
    };

    return std::make_shared<MessageListener>();
}

std::shared_ptr<std::span<int8_t>> ContactListener::onCallback(int type, const std::span<int8_t>* args)
{
    int64_t platformHandle = getPlatformHandle();
    auto ret = crosspl::proxy::ContactListener::onCallback(platformHandle, type, args);

    return ret;
}


