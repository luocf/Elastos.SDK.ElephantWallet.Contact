/**
 * @file	Contact_Factory.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_JNI_CONTACT_FACTORY_HPP_
#define _ELASTOS_SDK_JNI_CONTACT_FACTORY_HPP_

#include <string>

class Contact_Factory {
public:
    /*** type define ***/

    /*** static function and variable ***/
    static void SetLogLevel(int level);
    static int SetLocalDataDir(const std::string& dir);

    /*** class function and variable ***/
    explicit Contact_Factory() = default;
    virtual ~Contact_Factory() = default;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_FACTORY_HPP_ */

