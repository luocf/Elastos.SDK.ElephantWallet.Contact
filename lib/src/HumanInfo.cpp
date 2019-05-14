//
//  HumanInfo.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <HumanInfo.hpp>

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
HumanInfo::HumanInfo(const std::string& dump)
    : mChainPubKey()
    , mDid()
    , mElaAddr()
    , mCarrierAddrMap()
    , mNickname()
    , mAvatar()
    , mGender()
    , mDescription()
{
}

HumanInfo::~HumanInfo()
{
}

int HumanInfo::updateInfo(Item item, const std::string& value)
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
