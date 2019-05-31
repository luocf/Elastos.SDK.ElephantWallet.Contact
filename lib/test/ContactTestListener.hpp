#ifndef _CONTACT_TEST_LISTENER_HPP_
#define _CONTACT_TEST_LISTENER_HPP_

#include <Elastos.SDK.Contact.hpp>

class ContactTestListener : public elastos::MessageManager::MessageListener {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ContactTestListener(std::shared_ptr<elastos::Contact> contact) : mContact(contact) {};
    virtual ~ContactTestListener() = default;

    virtual void onStatusChanged(std::shared_ptr<elastos::UserInfo> userInfo,
                                 elastos::MessageManager::ChannelType channelType,
                                 elastos::UserInfo::Status status) override;

    virtual void onReceivedMessage(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                   elastos::MessageManager::ChannelType channelType,
                                   const std::shared_ptr<elastos::MessageManager::MessageInfo> msgInfo) override;

    virtual void onSentMessage(int msgIndex, int errCode) override;

    virtual void onFriendRequest(std::shared_ptr<elastos::FriendInfo> friendId,
                                 elastos::MessageManager::ChannelType channelType,
                                 const std::string& summary) override;

    virtual void onFriendStatusChanged(std::shared_ptr<elastos::FriendInfo> friendId,
                                       elastos::MessageManager::ChannelType channelType,
                                       elastos::FriendInfo::Status status) override;


protected:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::shared_ptr<elastos::Contact> mContact;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

#endif /* _CONTACT_TEST_LISTENER_HPP_ */

