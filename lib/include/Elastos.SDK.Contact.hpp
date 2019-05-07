/**
 * @file	Elastos.SDK.Contact.hpp
 * @brief	Contact
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_CONTACT_HPP_
#define _ELASTOS_SDK_CONTACT_HPP_

#include "FriendManager.hpp"
#include "MessageManager.hpp"
#include "UserManager.hpp"

namespace elastos {

class Contact {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit Contact(const char* userPubKey);
    explicit Contact();
    virtual ~Contact();

    const UserManager& getUserManager();
    const FriendManager& getFriendManager();
    const MessageManager& getMessageManager();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    UserManager mUserManager;
    FriendManager mFriendManager;
    MessageManager mMessageManager;

}; // class Contact

} // namespace elastos

#endif /* _ELASTOS_SDK_CONTACT_HPP_ */
