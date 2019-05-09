//
//  IdentifyCode.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <IdentifyCode.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
IdentifyCode::IdentifyCode(const std::string& phoneNumber,
                           const std::string& emailAddress,
                           const std::string& wechatId)
    : mPhoneNumber(phoneNumber)
    , mEmailAddress(emailAddress)
    , mWechatId(wechatId)

{
}

IdentifyCode::IdentifyCode()
    : IdentifyCode("", "", "")
{
}

IdentifyCode::~IdentifyCode()
{
}

int IdentifyCode::set(Type type, const std::string& value)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

std::string IdentifyCode::get(Type type) const
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
