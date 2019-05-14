/**
 * @file	ChannelImplCarrier.hpp
 * @brief	ChannelImplCarrier
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_CHANNEL_IMPL_CARRIER_HPP_
#define _ELASTOS_CHANNEL_IMPL_CARRIER_HPP_

#include <Config.hpp>
#include <ela_carrier.h>
#include <ela_session.h>
#include <MessageChannelStrategy.hpp>
#include <ThreadPool.hpp>

namespace elastos {

class ChannelImplCarrier : public MessageChannelStrategy {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ChannelImplCarrier(std::weak_ptr<Config> config);
    virtual ~ChannelImplCarrier();

    virtual int open() override;
    virtual int close() override;

    virtual int isReady() override;

    virtual int sendMessage(FriendInfo friendInfo,
                            int msgType, std::string msgContent) override;
    virtual int sendMessage(FriendInfo friendInfo,
                            int msgType, std::vector<int8_t> msgContent) override;

protected:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    void runCarrier();

    std::weak_ptr<Config> mConfig;
    std::unique_ptr<ElaCarrier, std::function<void(ElaCarrier*)>> mCarrier;
    std::unique_ptr<ThreadPool> mTaskThread;

}; // class ChannelImplCarrier

} // namespace elastos

#endif /* _ELASTOS_CHANNEL_IMPL_CARRIER_HPP_ */
