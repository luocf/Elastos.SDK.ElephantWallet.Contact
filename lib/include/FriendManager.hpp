/**
 * @file	FriendManager.hpp
 * @brief	FriendManager
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_FRIEND_MANAGER_HPP_
#define _ELASTOS_FRIEND_MANAGER_HPP_

#include "FriendInfo.hpp"
#include "IdentifyCode.hpp"

namespace elastos {

class FriendManager {
public:
    /*** type define ***/
    class FriendListener {
    public:
        enum Status {
            Online,
            Offline,
        };

        explicit FriendListener() = default;
        virtual ~FriendListener() = default;

        virtual int onRequest(FriendInfo friendInfo, std::string message) = 0;
        virtual void onStatusChanged(FriendInfo friendInfo, Status status) = 0;
        virtual void onReceiveMessage(FriendInfo friendInfo, int msgType, std::vector<int8_t> msgContent) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit FriendManager();
    virtual ~FriendManager();

    virtual void setFriendListner(FriendListener* listener);

    virtual int addFriendByDid(std::string did);
    virtual int addFriendByCarrier(std::string carrierAddress);
    virtual int addFriendByEla(std::string elaAddress);

    virtual std::string findFriend(int type, std::string content);
    virtual std::vector<FriendInfo> filterFriends(std::string regex);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::vector<FriendInfo> mFriendList;

    std::unique_ptr<FriendListener> mFriendListener;

}; // class FriendManager

} // namespace elastos

#endif /* _ELASTOS_FRIEND_MANAGER_HPP_ */
