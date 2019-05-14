/**
 * @file	ChannelImplElaChain.hpp
 * @brief	ChannelImplElaChain
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_CHANNEL_IMPL_ELACHAIN_HPP_
#define _ELASTOS_CHANNEL_IMPL_ELACHAIN_HPP_

#include <Config.hpp>
#include <MessageChannelStrategy.hpp>
#include <SecurityManager.hpp>
#include <ThreadPool.hpp>

namespace elastos {

class ChannelImplElaChain : public MessageChannelStrategy {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ChannelImplElaChain(std::weak_ptr<Config> config,
                                 std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~ChannelImplElaChain();

    virtual int open() override;
    virtual int close() override;

    virtual int isReady() override;

    virtual int sendMessage(FriendInfo friendInfo,
                            int msgType, std::string msgContent) override;
    virtual int sendMessage(FriendInfo friendInfo,
                            int msgType, std::vector<int8_t> msgContent) override;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/

}; // class ChannelImplElaChain

} // namespace elastos

#endif /* _ELASTOS_CHANNEL_IMPL_ELACHAIN_HPP_ */
