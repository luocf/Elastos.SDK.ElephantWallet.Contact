/**
 * @file	ContactDataListener.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_JNI_CONTACT_DATA_LISTENER_HPP_
#define _ELASTOS_SDK_JNI_CONTACT_DATA_LISTENER_HPP_

#include <mutex>

#include <CrossBase.hpp>
#include <Elastos.SDK.Contact.hpp>
#include <ContactListener.hpp>

class ContactDataListener : public CrossBase {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ContactDataListener();
    virtual ~ContactDataListener();

    std::shared_ptr<elastos::MessageManager::DataListener> getDataListener();

private:
    /*** type define ***/

    /*** static function and variable ***/
    static ContactDataListener* sContactDataListenerInstance;

    /*** class function and variable ***/
    std::shared_ptr<elastos::MessageManager::DataListener> makeDataListener();

    void onResult(const std::string& humanCode, ContactListener::ContactChannel channelType,
                  const std::string& dataId, int errCode);
    std::shared_ptr<std::span<uint8_t>> onReadData(const std::string& humanCode, ContactListener::ContactChannel channelType,
                                                   const std::string& dataId, uint64_t offset);
    int onWriteData(const std::string& humanCode, ContactListener::ContactChannel channelType,
                    const std::string& dataId, uint64_t offset, const std::span<uint8_t>* data);

    std::shared_ptr<elastos::MessageManager::DataListener> mDataListener;
}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_DATA_LISTENER_HPP_ */
