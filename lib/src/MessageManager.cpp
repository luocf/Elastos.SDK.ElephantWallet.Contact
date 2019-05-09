//
//  MessageManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <MessageManager.hpp>

#include <ChannelImplCarrier.hpp>
#include <ChannelImplElaChain.hpp>

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

int MessageManager::sendMessage(FriendInfo friendInfo, int msgType, std::string msgContent)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int MessageManager::sendMessage(FriendInfo friendInfo, int msgType, std::vector<int8_t> msgContent)
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
