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
    enum class RequestType: int {
        PublicKey = 201,
        EncryptData = 202,
        DecryptData = 203,
        DidPropAppId = 204,
        DidAgentAuthHeader = 205,
        SignData = 206,
    };

    enum class EventType: int {
        StatusChanged = 101,
        ReceivedMessage = 102,
        SentMessage = 103,
        FriendRequest = 104,
        FriendStatusChanged = 105,
    };

    enum class ContactChannel: int {
        Carrier = 1,
        ElaChain = 2,
        Email = 3,
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ContactListener();
    virtual ~ContactListener();

    std::shared_ptr<elastos::SecurityManager::SecurityListener> getSecurityListener();
    std::shared_ptr<elastos::MessageManager::MessageListener> getMessageListener();

private:
    /*** type define ***/

    /*** static function and variable ***/
    static ContactListener* sContactListenerInstance;

    /*** class function and variable ***/
    std::shared_ptr<elastos::SecurityManager::SecurityListener> makeSecurityListener();
    std::shared_ptr<elastos::MessageManager::MessageListener> makeMessageListener();
    std::shared_ptr<std::span<uint8_t>> onRequest(RequestType type, const char* pubKey, const std::span<uint8_t>* data);
    void onEvent(EventType type, const std::string& humanCode, ContactChannel channelType, const std::span<uint8_t>* data);

    std::shared_ptr<elastos::SecurityManager::SecurityListener> mSecurityListener;
    std::shared_ptr<elastos::MessageManager::MessageListener> mMessageListener;
}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_LISTENER_HPP_ */
