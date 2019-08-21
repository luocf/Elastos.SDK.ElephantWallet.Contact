//
//  PlatformAndroid.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#if defined(__ANDROID__)

#include "PlatformAndroid.hpp"
#include <dlfcn.h>
#include <iomanip>
#include <sstream>
#include "ErrCode.hpp"
#include "Log.hpp"

#include "CrossPLUtils.hpp"

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
std::string PlatformAndroid::mCurrentDevId;

/***********************************************/
/***** static function implement ***************/
/***********************************************/
void PlatformAndroid::DetachCurrentThread()
{
    crosspl::CrossPLUtils::DetachCurrentThread();
}

std::string PlatformAndroid::GetBacktrace()
{
    const size_t max = 30;
    void* buffer[max];

    BacktraceState state = {buffer, buffer + max};
    _Unwind_Backtrace(UnwindCallback, &state);
    int count = state.current - buffer;

    std::stringstream sstream;
    for (size_t idx = 0; idx < count; ++idx) {
        const void* addr = buffer[idx];
        const char* symbol = "";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname) {
            symbol = info.dli_sname;
        }

        sstream << "  #" << std::setw(2) << idx << ": " << addr << "  " << symbol << std::endl;
    }

    return sstream.str();
}

int PlatformAndroid::GetCurrentDevId(std::string& devId)
{
    if(mCurrentDevId.empty()) {
        return ErrCode::DevUUIDError;
    }

    devId = mCurrentDevId;
    return 0;
}

int PlatformAndroid::GetCurrentDevName(std::string& devName)
{
    devName = "Android";
    return 0;
}

void PlatformAndroid::SetCurrentDevId(const std::string& devId)
{
    mCurrentDevId = devId;
}

_Unwind_Reason_Code PlatformAndroid::UnwindCallback(struct _Unwind_Context* context, void* arg)
{
    BacktraceState* state = static_cast<BacktraceState*>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc) {
        if (state->current == state->end) {
            return _URC_END_OF_STACK;
        } else {
            *state->current++ = reinterpret_cast<void*>(pc);
        }
    }
    return _URC_NO_REASON;
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

#endif /* defined(__ANDROID__) */
