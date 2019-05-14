#include <Elastos.SDK.Contact.hpp>
#include <Elastos.SDK.Keypair.C/Elastos.Wallet.Utility.h>
#include <iostream>
#include <execinfo.h>
#include <signal.h>

std::string gSavedMnemonic;
std::string gCachedMnemonic;

void signalHandler(int sig);
std::shared_ptr<elastos::SecurityManager::SecurityListener> getSecurityListener(std::weak_ptr<elastos::Contact> contact);
std::shared_ptr<elastos::UserManager::UserListener> getUserListener();
std::shared_ptr<elastos::FriendManager::FriendListener> getFriendListener();
std::shared_ptr<elastos::MessageManager::MessageListener> getMessageListener();
std::string getPublicKey();

int main(int argc, char **argv)
{
    signal(SIGSEGV, signalHandler);

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

void signalHandler(int sig) {
    std::cerr << "Error: signal " << sig << std::endl;

    void* addrlist[10];
    int addrlen = backtrace( addrlist, sizeof( addrlist ) / sizeof( void* ));
    if(addrlen == 0) {
        std::cerr << std::endl;
        return;
    }

    char** symbollist = backtrace_symbols( addrlist, addrlen );
    for ( int i = 4; i < addrlen; i++ ) {
        std::cerr << symbollist[i] << std::endl;
    }
    free(symbollist);

    exit(1);
}

std::shared_ptr<elastos::SecurityManager::SecurityListener> getSecurityListener(std::weak_ptr<elastos::Contact> contact)
{
    class SecurityListener final : public elastos::SecurityManager::SecurityListener {
    public:
        explicit SecurityListener(std::weak_ptr<elastos::Contact> contact)
            : mContact(contact) {
        };
        virtual ~SecurityListener() = default;

        std::string onRequestPublicKey() override {
            auto pubKey = getPublicKey();
            std::cout << __PRETTY_FUNCTION__ << " pubKey:" << pubKey << std::endl;
            return pubKey;
        };

        std::vector<int8_t> onEncryptData(const std::string& pubKey, const std::vector<int8_t>& src) override {
            return src;
        }
        std::vector<int8_t> onDecryptData(const std::vector<int8_t>& src) override {
            return src;
        }

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

        virtual void onReceivedMessage(const elastos::FriendInfo& friendInfo,
                                       elastos::MessageManager::ChannelType chType,
                                       const elastos::MessageManager::MessageInfo& msgInfo) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        };

        virtual void onSentMessage(int msgIndex, int errCode) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        };
    };

    return std::make_shared<MessageListener>();
}

std::string getPublicKey()
{
    const char* language = "english";
    const char* words = "";

    if(gCachedMnemonic.empty() == true) {
        std::string password;
        std::cout << "input password: ";
        std::getline(std::cin, password);

        if(gSavedMnemonic.empty() == true) {
            gSavedMnemonic = ::generateMnemonic(language, words);
            std::cout << "generate mnemonic: " << gSavedMnemonic << std::endl;
        }
        gCachedMnemonic = gSavedMnemonic;
    }

    void* seedData = nullptr;
    int seedSize = ::getSeedFromMnemonic(&seedData, gCachedMnemonic.c_str(), language, words, "");

    auto pubKey = ::getSinglePublicKey(seedData, seedSize);
    freeBuf(seedData);

    std::string retval = pubKey;
    freeBuf(pubKey);

    return pubKey;

}
