//
//  ChannelImplElaChain.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

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
ChannelImplElaChain::ChannelImplElaChain(std::weak_ptr<Config> config,
                                         std::weak_ptr<SecurityManager> sectyMgr)
    : MessageChannelStrategy()
    , mConfig(config)
    , mSecurityManager(sectyMgr)
    , mTaskThread()
{
}

ChannelImplElaChain::~ChannelImplElaChain()
{
}

int ChannelImplElaChain::open()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int ChannelImplElaChain::close()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int ChannelImplElaChain::sendMessage(FriendInfo friendInfo,
                                     int msgType, std::string msgContent)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}
int ChannelImplElaChain::sendMessage(FriendInfo friendInfo,
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
