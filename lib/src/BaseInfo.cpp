//
//  BaseInfo.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <BaseInfo.hpp>

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
BaseInfo::BaseInfo(const std::string& did)
    : mDid(did)
    , mElaAddr()
    , mCarrierAddrMap()
    , mNickname()
    , mAvatar()
    , mGender()
    , mDescription()
{
}

BaseInfo::~BaseInfo()
{
}

int BaseInfo::updateInfo(Item item, const std::string& value)
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
