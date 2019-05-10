/**
 * @file	MessageChannelStrategy.hpp
 * @brief	MessageChannelStrategy
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_MESSAGE_CHANNEL_STRATEGY_HPP_
#define _ELASTOS_MESSAGE_CHANNEL_STRATEGY_HPP_

#include <cstdint>
#include <UserInfo.hpp>
#include <FriendInfo.hpp>

namespace elastos {

class MessageChannelStrategy {
public:
    /*** type define ***/
    class ChannelListener {
    public:
        explicit ChannelListener() = default;
        virtual ~ChannelListener() = default;

        virtual void onReceivedMessage(UserInfo userInfo, FriendInfo friendInfo,
                                       int channelType,
                                       int msgType, std::vector<int8_t> msgContent) = 0;
        virtual void onSentMessage(int msgIndex, int errCode) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    virtual void setChannelListener(std::shared_ptr<ChannelListener> listener);

    virtual int open() = 0;
    virtual int close() = 0;

    virtual int sendMessage(FriendInfo friendInfo,
                            int msgType, std::string msgContent) = 0;
    virtual int sendMessage(FriendInfo friendInfo,
                            int msgType, std::vector<int8_t> msgContent) = 0;

protected:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit MessageChannelStrategy();
    virtual ~MessageChannelStrategy();

    std::shared_ptr<ChannelListener> mChannelListener;

}; // class MessageChannelStrategy

} // namespace elastos

#endif /* _ELASTOS_MESSAGE_CHANNEL_STRATEGY_HPP_ */
