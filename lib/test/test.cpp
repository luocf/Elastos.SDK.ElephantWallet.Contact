#include <Elastos.SDK.Contact.hpp>
#include <iostream>

std::string gSavedMnemonic;
std::string gCachedMnemonic;

std::shared_ptr<elastos::SecurityManager::SecurityListener> getSecurityListener(std::weak_ptr<elastos::Contact> contact);
std::shared_ptr<elastos::UserManager::UserListener> getUserListener();
std::shared_ptr<elastos::FriendManager::FriendListener> getFriendListener();
std::shared_ptr<elastos::MessageManager::MessageListener> getMessageListener();

int main(int argc, char **argv)
{
    int ret = elastos::Contact::Factory::SetLocalDataDir("/tmp/elastos.sdk.contact/test");
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to set contact local dir! ret=") + std::to_string(ret));
    }

    auto contact = elastos::Contact::Factory::Create();

    auto securityListener = getSecurityListener(contact);
    auto userListener = getUserListener();
    auto friendListener = getFriendListener();
    auto msgListener = getMessageListener();
    contact->setListener(securityListener, userListener, friendListener, msgListener);

    ret = contact->start();
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to start contact! ret=") + std::to_string(ret));
    }

    return 0;
}

std::shared_ptr<elastos::SecurityManager::SecurityListener> getSecurityListener(std::weak_ptr<elastos::Contact> contact)
{
    class SecurityListener : public elastos::SecurityManager::SecurityListener {
    public:
        explicit SecurityListener(std::weak_ptr<elastos::Contact> contact)
            : mContact(contact) {
        };
        virtual ~SecurityListener() = default;

        virtual std::string onRequestMnemonic() override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            if(gSavedMnemonic.empty() == true) {
                auto sectyMgr = mContact.lock()->getSecurityManager();
                sectyMgr.lock()->generateMnemonic(gSavedMnemonic);
                std::cout << "generate mnemonic: " << gSavedMnemonic << std::endl;
            }

            if(gCachedMnemonic.empty() == true) {
                std::string password;
                std::cout << "input password: ";
                std::getline(std::cin, password);

                gCachedMnemonic = gSavedMnemonic;
            }

            return gCachedMnemonic;
        };

    private:
        std::weak_ptr<elastos::Contact> mContact;
    };

    return std::make_shared<SecurityListener>(contact);
}


std::shared_ptr<elastos::UserManager::UserListener> getUserListener()
{
    class UserListener : public elastos::UserManager::UserListener {
    public:
        explicit UserListener() = default;
        virtual ~UserListener() = default;

        virtual void onStatusChanged(const std::weak_ptr<elastos::UserInfo> userInfo, elastos::UserManager::Status status) override {
            std::cout << __PRETTY_FUNCTION__ << " Status:" << status << std::endl;
            if(status == elastos::UserManager::Status::Ready) {
                gCachedMnemonic.clear();
            }
        };

        virtual int onSigninDevicesOverflow(const std::weak_ptr<elastos::UserInfo> userInfo, int capacity) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            return 0;
        };
    };

    return std::make_shared<UserListener>();
}

std::shared_ptr<elastos::FriendManager::FriendListener> getFriendListener()
{
    class FriendListener : public elastos::FriendManager::FriendListener {
    public:
        explicit FriendListener() = default;
        virtual ~FriendListener() = default;

        virtual int onRequest(elastos::FriendInfo friendInfo, std::string message) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            return 0;
        };

        virtual void onStatusChanged(elastos::FriendInfo friendInfo, elastos::FriendManager::Status status) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        };
    };

    return std::make_shared<FriendListener>();
}

std::shared_ptr<elastos::MessageManager::MessageListener> getMessageListener()
{
    class MessageListener : public elastos::MessageManager::MessageListener {
    public:
        explicit MessageListener() = default;
        virtual ~MessageListener() = default;

        virtual void onReceivedMessage(elastos::UserInfo userInfo, elastos::FriendInfo friendInfo,
                                       int channelType,
                                       int msgType, std::vector<int8_t> msgContent) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        };

        virtual void onSentMessage(int msgIndex, int errCode) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        };
    };

    return std::make_shared<MessageListener>();
}
