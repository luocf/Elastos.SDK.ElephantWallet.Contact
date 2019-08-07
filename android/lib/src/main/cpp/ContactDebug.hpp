/**
 * @file	ContactDebug.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_JNI_CONTACT_DEBUG_HPP_
#define _ELASTOS_SDK_JNI_CONTACT_DEBUG_HPP_

#include <sstream>

class ContactDebug {
public:
    /*** type define ***/

    /*** static function and variable ***/
    static int GetCachedDidProp(std::stringstream* value);

    /*** class function and variable ***/
    explicit ContactDebug();
    virtual ~ContactDebug();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_DEBUG_HPP_ */

