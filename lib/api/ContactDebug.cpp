//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactDebug.hpp>

#include "DidChnClient.hpp"
#include "Log.hpp"

#ifdef WITH_CROSSPL
namespace crosspl {
namespace native {
#endif // WITH_CROSSPL
    
/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/

int ContactDebug::GetCachedDidProp(std::stringstream* value)
{
    auto dcClient = elastos::DidChnClient::GetInstance();
    if(dcClient.get() == nullptr) {
        return elastos::ErrCode::NotReadyError;
    }

    std::string cachedDidProp;
    int ret = dcClient->printCachedDidProp(cachedDidProp);
    CHECK_ERROR(ret);

    value->str(cachedDidProp);

    return 0;
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


#ifdef WITH_CROSSPL
} //namespace native
} //namespace crosspl
#endif // WITH_CROSSPL
