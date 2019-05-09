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
#include "SecurityManager.hpp"

namespace elastos {

class FriendManager {
public:
    /*** type define ***/
    enum Status {
        Online,
        Offline,
    };

    class FriendListener {
    public:
        explicit FriendListener() = default;
        virtual ~FriendListener() = default;

        virtual int onRequest(FriendInfo friendInfo, std::string message) = 0;
        virtual void onStatusChanged(FriendInfo friendInfo, Status status) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit FriendManager(std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~FriendManager();

    virtual void setFriendListener(std::shared_ptr<FriendListener> listener);

    virtual int addFriendByDid(std::string did);
    virtual int addFriendByCarrier(std::string carrierAddress);
    virtual int addFriendByEla(std::string elaAddress);

    virtual int removeFriendByDid(std::string did);
    virtual int removeFriendByCarrier(std::string carrierAddress);
    virtual int removeFriendByEla(std::string elaAddress);

    virtual std::string findFriend(int type, std::string content);
    virtual std::vector<FriendInfo> filterFriends(std::string regex);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::weak_ptr<SecurityManager> mSecurityManager;
    std::shared_ptr<FriendListener> mFriendListener;
    std::vector<FriendInfo> mFriendList;

}; // class FriendManager

} // namespace elastos

#endif /* _ELASTOS_FRIEND_MANAGER_HPP_ */
