/**
 * @file	PlatformDarwin.hpp
 * @brief	PlatformDarwin
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_PLATFORM_HPP_
#define _ELASTOS_PLATFORM_HPP_

#if defined(__APPLE__)
#include "PlatformDarwin.hpp"
#elif defined(__ANDROID__)
#include "PlatformAndroid.hpp"
#else
#error "Unsupport Platform"
#endif

namespace elastos {

#if defined(__APPLE__)
class Platform : public PlatformDarwin {
#elif defined(__ANDROID__)
class Platform : public PlatformAndroid {
#else
#error "Unsupport Platform"
#endif
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit Platform() = delete;
    virtual ~Platform() = delete;

}; // class PlatformDarwin

} // namespace elastos

#endif /* _ELASTOS_PLATFORM_HPP_ */
