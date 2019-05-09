/**
 * @file	UserManager.hpp
 * @brief	UserManager
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_USER_MANAGER_HPP_
#define _ELASTOS_USER_MANAGER_HPP_

#include "FriendInfo.hpp"
#include "IdentifyCode.hpp"
#include "SecurityManager.hpp"
#include "UserInfo.hpp"

namespace elastos {

class UserManager {
public:
    /*** type define ***/
    enum Status {
        Pending = 1,
        Ready,
        Online,
        Offline,
    };

    class UserListener {
    public:

        explicit UserListener() = default;
        virtual ~UserListener() = default;

        virtual void onStatusChanged(const std::weak_ptr<UserInfo> userInfo, Status status) = 0;
        virtual int onSigninDevicesOverflow(const std::weak_ptr<UserInfo> userInfo, int capacity) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit UserManager(std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~UserManager();

    virtual void setUserListener(std::shared_ptr<UserListener> listener);

    int makeUser();

    Status getStatus();

    int updateUserInfo(IdentifyCode::Type idType, const std::string& value);
    std::weak_ptr<UserInfo> getUserInfo() const;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    int syncHistoryInfo();
    void setStatus(Status status);

    std::weak_ptr<SecurityManager> mSecurityManager;
    std::shared_ptr<UserListener> mUserListener;
    std::shared_ptr<UserInfo> mUserInfo;

    Status mStatus;
}; // class UserManager

} // namespace elastos

#endif /* _ELASTOS_USER_MANAGER_HPP_ */
