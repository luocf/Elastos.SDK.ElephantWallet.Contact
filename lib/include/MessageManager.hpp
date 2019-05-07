/**
 * @file	MessageManager.hpp
 * @brief	MessageManager
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_MANAGER_MANAGER_HPP_
#define _ELASTOS_MANAGER_MANAGER_HPP_

#include "FriendInfo.hpp"
#include "UserInfo.hpp"

namespace elastos {

class MessageManager {
public:
    /*** type define ***/
    class MessageListener {
    public:
        explicit MessageListener() = default;
        virtual ~MessageListener() = default;

        virtual void onReceiveMessage(UserInfo userInfo, FriendInfo friendInfo,
                                      int channelType,
                                      int msgType, std::vector<int8_t> msgContent) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit MessageManager(std::weak_ptr<UserInfo> userInfo);
    virtual ~MessageManager();

    virtual void setMessageListner(MessageListener* listener);

    virtual void sendMessage(FriendInfo friendInfo, int msgType, std::string msgContent);
    virtual void sendMessage(FriendInfo friendInfo, int msgType, std::vector<int8_t> msgContent);

    virtual void sendChainMessage(FriendInfo friendInfo, int msgType, std::vector<int8_t> msgContent,
                                  const std::string& seed) = 0;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::weak_ptr<UserInfo> mUserInfo;

}; // class MessageManager

} // namespace elastos

#endif /* _ELASTOS_MANAGER_MANAGER_HPP_ */
