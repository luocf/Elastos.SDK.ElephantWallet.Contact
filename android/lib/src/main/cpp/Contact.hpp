/**
 * @file	Contact.hpp
 * @brief	Contact
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_JNI_CONTACT_HPP_
#define _ELASTOS_SDK_JNI_CONTACT_HPP_

#include <CrossBase.hpp>
#include <ContactListener.hpp>
#include "ContactListener.hpp"

class Contact {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit Contact();
    virtual ~Contact();

    void setListener(CrossBase* listener);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    ContactListener* mListener;

}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_HPP_ */
