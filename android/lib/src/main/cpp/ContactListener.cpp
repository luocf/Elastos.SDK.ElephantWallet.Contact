//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactListener.hpp>

#include <ContactListener.proxy.hpp>
#include "Log.hpp"

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ContactListener::ContactListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}
ContactListener::~ContactListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}

std::shared_ptr<std::span<int8_t>> ContactListener::onCallback(int type, const std::span<int8_t>* args)
{
    int64_t platformHandle = getPlatformHandle();
    auto ret = crosspl::proxy::ContactListener::onCallback(platformHandle, type, args);

    return ret;
}


/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
