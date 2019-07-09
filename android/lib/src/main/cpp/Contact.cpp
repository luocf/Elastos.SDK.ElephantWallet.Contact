//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <Contact.hpp>

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
Contact::Contact()
        : mContactImpl()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mContactImpl = elastos::Contact::Factory::Create();
}
Contact::~Contact()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}

void Contact::setListener(CrossBase* listener)
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mListener = dynamic_cast<ContactListener*>(listener);
//    mListener->onCallback(0, nullptr);

    auto sectyListener = mListener->getSecurityListener();
    auto msgListener = mListener->getMessageListener();
    mContactImpl->setListener(sectyListener, nullptr, nullptr, msgListener);

    return;
}

int Contact::start()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    int ret = mContactImpl->start();

    return ret;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
