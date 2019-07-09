//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <Contact.hpp>

#include <Elastos.SDK.Contact.hpp>
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
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}
Contact::~Contact()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}

void Contact::setListener(CrossBase* listener)
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    mListener = dynamic_cast<ContactListener*>(listener);

    return;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
