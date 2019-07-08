//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <Contact_Factory.hpp>

#include <Elastos.SDK.Contact.hpp>

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/

void Contact_Factory::SetLogLevel(int level)
{
    elastos::Contact::Factory::SetLogLevel(level);

    return;
}

int Contact_Factory::SetLocalDataDir(const std::string& dir)
{
    int ret = elastos::Contact::Factory::SetLocalDataDir(dir);

    return ret;
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
