//
//  ErrCode.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ErrCode.hpp>

#include <system_error>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/
std::string ErrCode::ToString(int errCode)
{
    std::string errMsg;

    switch (errCode) {
    case UnknownError:
        errMsg = "UnknownError";
        break;
    case InvalidArgument:
        errMsg = "InvalidArgument";
        break;
    case IOSystemException:
        errMsg = "IOSystemException";
        break;
    case NetworkException:
        errMsg = "NetworkException";
        break;
    case InvalidLocalDataDir:
        errMsg = "InvalidLocalDataDir";
        break;
    case NoSecurityListener:
        errMsg = "NoSecurityListener";
        break;
    case BadSecurityValue:
        errMsg = "BadSecurityValue";
        break;
    case StdSystemErrorIndex:
        errMsg = "StdSystemErrorIndex";
        break;
    }

    if(errCode < StdSystemErrorIndex) {
        int stdErrVal = StdSystemErrorIndex - errCode;
        auto stdErrCode = std::error_code(stdErrVal, std::generic_category());
        errMsg = stdErrCode.message();
    }

    return errMsg;
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

} // namespace elastos
