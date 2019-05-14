//
//  MessageManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <MessageManager.hpp>

#include <ChannelImplCarrier.hpp>
#include <ChannelImplElaChain.hpp>
#include <Random.hpp>

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
MessageManager::MessageManager(std::weak_ptr<SecurityManager> sectyMgr)
    : mSecurityManager(sectyMgr)
    , mMessageListener()
    , mUserInfo()
    , mMessageChannelMap()
{
}

MessageManager::~MessageManager()
{
}

void MessageManager::setMessageListener(std::shared_ptr<MessageListener> listener)
{
    mMessageListener = listener;
}

int MessageManager::openChannels(std::weak_ptr<Config> config)
{
    mMessageChannelMap[ChannelType::Carrier] = std::make_shared<ChannelImplCarrier>(config);
    mMessageChannelMap[ChannelType::ElaChain] = std::make_shared<ChannelImplElaChain>(config, mSecurityManager);

    for(const auto& channel: mMessageChannelMap) {
        channel.second->open();
    }

    return 0;
}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::makeMessage(MessageType type,
                                                                         const std::vector<uint8_t>& plainContent,
                                                                         const std::string& cryptoAlgorithm) const
{
    struct Impl: MessageManager::MessageInfo {
        Impl(MessageType type,
             const std::vector<uint8_t>& plainContent,
             const std::string& cryptoAlgorithm)
            : MessageManager::MessageInfo(type, plainContent, cryptoAlgorithm) {
        }
    };

    auto msgInfo = std::make_shared<Impl>(type, plainContent, cryptoAlgorithm);

    return msgInfo;
}

std::shared_ptr<MessageManager::MessageInfo> MessageManager::makeMessage(const std::string& plainContent,
                                                                         const std::string& cryptoAlgorithm) const
{
    std::vector<uint8_t> plainContentBytes(plainContent.begin(), plainContent.end());

    return makeMessage(MessageType::Text, plainContentBytes, cryptoAlgorithm);
}

int MessageManager::sendMessage(const FriendInfo& friendInfo, ChannelType chType, const MessageInfo& msgInfo)
{
    auto it = mMessageChannelMap.find(chType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::ChannelNotFound;
    }
    auto channel = it->second;

    if(channel->isReady() == false) {
        return ErrCode::ChannelNotReady;
    }

    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
MessageManager::MessageInfo::MessageInfo(MessageType type,
                                         const std::vector<uint8_t>& plainContent,
                                         const std::string& cryptoAlgorithm)
    : mId(Random::Gen<uint32_t>())
    , mType(type)
    , mPlainContent(plainContent)
    , mCryptoAlgorithm(cryptoAlgorithm)
{
}

MessageManager::MessageInfo::~MessageInfo()
{
}

} // namespace elastos
