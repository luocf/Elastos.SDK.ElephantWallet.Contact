/**
 * @file	ContactListener.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_JNI_CONTACT_LISTENER_HPP_
#define _ELASTOS_SDK_JNI_CONTACT_LISTENER_HPP_

#include <mutex>

#include <CrossBase.hpp>
#include <Elastos.SDK.Contact.hpp>

class ContactListener : public CrossBase {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ContactListener();
    virtual ~ContactListener();

    std::shared_ptr<elastos::SecurityManager::SecurityListener> getSecurityListener();
    std::shared_ptr<elastos::MessageManager::MessageListener> getMessageListener();

private:
    /*** type define ***/
    struct CallbackType {
        static constexpr int Error = -10000;
        static constexpr int Request = 20000;
        static constexpr int Event = 30000;

        // request
        static constexpr int PublicKey = 1;
        static constexpr int EncryptData = 2;
        static constexpr int DecryptData = 3;
        static constexpr int DidPropAppId = 4;
        static constexpr int DidAgentAuthHeader = 5;
        static constexpr int SignData = 6;


        static constexpr int StatusChanged = 10;
        static constexpr int ReceivedMessage = 11;
        static constexpr int SentMessage = 12;
        static constexpr int FriendRequest = 13;
        static constexpr int FriendStatusChanged = 14;

        static int PackError(int code) { return (code + Error); }
        static int PackRequest(int code) { return (code + Request); }
        static int PackEvent(int code) { return (code + Event); }
    };

    /*** static function and variable ***/
    static std::recursive_mutex sContactListenerMutex;
    static ContactListener* sContactListenerInstance;

    /*** class function and variable ***/
    std::shared_ptr<elastos::SecurityManager::SecurityListener> makeSecurityListener();
    std::shared_ptr<elastos::MessageManager::MessageListener> makeMessageListener();
    std::shared_ptr<std::span<int8_t>> onCallback(int type, const std::span<int8_t>* args);

    std::shared_ptr<elastos::SecurityManager::SecurityListener> mSecurityListener;
    std::shared_ptr<elastos::MessageManager::MessageListener> mMessageListener;
}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_LISTENER_HPP_ */
