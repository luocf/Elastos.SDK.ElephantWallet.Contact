/**
 * @file	Contact.hpp
 * @brief	Contact
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_JNI_CONTACT_BRIDGE_HPP_
#define _ELASTOS_SDK_JNI_CONTACT_BRIDGE_HPP_

#include <sstream>

#include <ContactListener.hpp>
#include <CrossBase.hpp>
#include <Elastos.SDK.Contact.hpp>

class ContactBridge {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ContactBridge();
    virtual ~ContactBridge();

    void setListener(CrossBase* listener);
    int start();

    int getHumanInfo(const char* humanCode, std::stringstream* info);
    int getHumanStatus(const char* humanCode);

    int addFriend(const char* friendCode, const char* summary);
    int acceptFriend(const char* friendCode);
    int getFriendList(std::stringstream* info);

    int syncInfoDownloadFromDidChain();
    int syncInfoUploadToDidChain();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::shared_ptr<elastos::Contact> mContactImpl;
    ContactListener* mListener;

}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_BRIDGE_HPP_ */
