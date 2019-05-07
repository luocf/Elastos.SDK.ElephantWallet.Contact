/**
 * @file	UserInfo.hpp
 * @brief	UserInfo
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_USER_INFO_HPP_
#define _ELASTOS_USER_INFO_HPP_

#include "BaseInfo.hpp"
#include "FriendInfo.hpp"
#include "IdentifyCode.hpp"

namespace elastos {

class UserInfo : public BaseInfo {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit UserInfo(const char* userPubKey);
    explicit UserInfo();
    virtual ~UserInfo();

    int setIdentifyCode(IdentifyCode::Type idType, const std::string& value);
    const IdentifyCode& getIdentifyCode();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    IdentifyCode mIdentifyCode;
}; // class UserInfo

} // namespace elastos

#endif /* _ELASTOS_USER_INFO_HPP_ */
