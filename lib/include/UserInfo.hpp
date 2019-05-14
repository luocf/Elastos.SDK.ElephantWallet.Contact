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

#include "HumanInfo.hpp"
#include "FriendInfo.hpp"
#include "IdentifyCode.hpp"

namespace elastos {

class UserInfo : public HumanInfo {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit UserInfo(const std::string& did);
    explicit UserInfo();
    virtual ~UserInfo();

    int setIdentifyCode(IdentifyCode::Type idType, const std::string& value);
    const IdentifyCode& getIdentifyCode() const;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    IdentifyCode mIdentifyCode;

}; // class UserInfo

} // namespace elastos

#endif /* _ELASTOS_USER_INFO_HPP_ */
