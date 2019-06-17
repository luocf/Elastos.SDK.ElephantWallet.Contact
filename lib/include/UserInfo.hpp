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

#include <memory>
#include "HumanInfo.hpp"
#include "IdentifyCode.hpp"

namespace elastos {

class UserManager;

class UserInfo : public HumanInfo, IdentifyCode {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit UserInfo(std::weak_ptr<UserManager> userMgr);
    virtual ~UserInfo();

    virtual int serialize(std::string& value, bool summaryOnly = false) const override;
    virtual int deserialize(const std::string& value, bool summaryOnly = false) override;

    virtual int addCarrierInfo(const CarrierInfo& info, const Status status) override;
    virtual int setHumanInfo(Item item, const std::string& value) override;

    virtual int setIdentifyCode(Type type, const std::string& value) override;
    virtual int getIdentifyCode(Type type, std::string& value) const override;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::weak_ptr<UserManager> mUserManager;

}; // class UserInfo

} // namespace elastos

#endif /* _ELASTOS_USER_INFO_HPP_ */
