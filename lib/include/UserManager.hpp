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

namespace elastos {

class UserManager {
public:
    /*** type define ***/
    class UserListener {
    public:
        enum Status {
            Ready,
            Online,
            Offline,
        };

        explicit UserListener() = default;
        virtual ~UserListener() = default;

        virtual void onStatusChanged(const UserInfo& userInfo, Status status) = 0;
        virtual int onSigninDevicesOverflow(const UserInfo& userInfo, int capacity) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit UserManager();
    virtual ~UserManager();

    void reset();

    std::string newUser();
    int recoverUser(const std::string& userPubKey);

    Status getStatus();

    int updateUserInfo(IdentifyCode::Type idType, const std::string& value);
    const UserInfo* getUserInfo() const;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    int syncHistoryInfo();

    std::unique_ptr<UserInfo> mUserInfo;
    Status mStatus;
}; // class UserManager

} // namespace elastos

#endif /* _ELASTOS_USER_MANAGER_HPP_ */
