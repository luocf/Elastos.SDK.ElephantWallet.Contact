/**
 * @file	MessageManager.hpp
 * @brief	MessageManager
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_MESSAGE_MANAGER_HPP_
#define _ELASTOS_MESSAGE_MANAGER_HPP_

#include "Config.hpp"
#include "ErrCode.hpp"
#include "FriendInfo.hpp"
#include "SecurityManager.hpp"
#include "UserInfo.hpp"

#include "MessageChannelStrategy.hpp"

namespace elastos {

class MessageManager {
public:
    /*** type define ***/
    enum class ChannelType: uint32_t {
        Carrier  = 0x000000FF,
        ElaChain = 0x00000100,
    };

    class MessageListener {
    public:
        explicit MessageListener() = default;
        virtual ~MessageListener() = default;

        virtual void onReceivedMessage(UserInfo userInfo, FriendInfo friendInfo,
                                       int channelType,
                                       int msgType, std::vector<int8_t> msgContent) = 0;
        virtual void onSentMessage(int msgIndex, int errCode) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit MessageManager(std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~MessageManager();

    virtual void setMessageListener(std::shared_ptr<MessageListener> listener);

    virtual int openChannels(std::weak_ptr<Config> config);

    virtual int sendMessage(FriendInfo friendInfo, int msgType, std::string msgContent);
    virtual int sendMessage(FriendInfo friendInfo, int msgType, std::vector<int8_t> msgContent);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::weak_ptr<SecurityManager> mSecurityManager;
    std::shared_ptr<MessageListener> mMessageListener;
    std::weak_ptr<UserInfo> mUserInfo;

    std::map<ChannelType, std::shared_ptr<MessageChannelStrategy>> mMessageChannelMap;

}; // class MessageManager

} // namespace elastos

#endif /* _ELASTOS_MESSAGE_MANAGER_HPP_ */
