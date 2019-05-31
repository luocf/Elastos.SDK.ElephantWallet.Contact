#include <Elastos.SDK.Contact.hpp>
#include <Elastos.SDK.Keypair.C/Elastos.Wallet.Utility.h>
#include <execinfo.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <signal.h>

#include <Log.hpp>
#include "ContactTestCmd.hpp"
#include "ContactTestListener.hpp"

std::string gSavedMnemonic;
std::string gCachedMnemonic;

void loop(const char* fifoFilePath, std::shared_ptr<elastos::Contact> contact);
void signalHandler(int sig);
std::shared_ptr<elastos::SecurityManager::SecurityListener> getSecurityListener(std::weak_ptr<elastos::Contact> contact);
std::shared_ptr<elastos::UserManager::UserListener> getUserListener();
std::shared_ptr<elastos::FriendManager::FriendListener> getFriendListener();
std::string getPublicKey();

int main(int argc, char **argv)
{
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);

    const char* nickname = "Friend";
    const char* fifoFilePath = (argc > 1 ? argv[1] : nullptr);
    if(fifoFilePath != nullptr) {
        gSavedMnemonic = "panda merry use liquid thank segment end federal possible work average diet";
        nickname = "Me";
    }

    Log::I(Log::TAG, "Start Contact Test.");
    Log::I(Log::TAG, "%s\n", (argc > 1 ? argv[1]:""));

    elastos::Contact::Factory::SetLogLevel(4);
    int ret = elastos::Contact::Factory::SetLocalDataDir("/tmp/elastos.sdk.contact/test");
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to set contact local dir! ret=") + std::to_string(ret));
    }

    auto contact = elastos::Contact::Factory::Create();

    auto sectyListener = getSecurityListener(contact);
    auto userListener = getUserListener();
    auto friendListener = getFriendListener();
    auto msgListener = std::make_shared<ContactTestListener>(contact);
    contact->setListener(sectyListener, userListener, friendListener, msgListener);

    ret = contact->start();
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to start contact! ret=") + std::to_string(ret));
    }

    gCachedMnemonic.clear();

    std::shared_ptr<elastos::UserInfo> userInfo;
    ret = contact->getUserManager().lock()->getUserInfo(userInfo);
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to get user info! ret=") + std::to_string(ret));
    }

    ret = userInfo->setHumanInfo(elastos::UserInfo::Item::Nickname, nickname);
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to get user info! ret=") + std::to_string(ret));
    }

    std::string value;
    userInfo->getHumanInfo(elastos::UserInfo::Item::Nickname, value);
    Log::I(Log::TAG, "NickName  : %s", value.c_str());
    userInfo->getHumanInfo(elastos::UserInfo::Item::ChainPubKey, value);
    Log::I(Log::TAG, "PubKey    : %s", value.c_str());
    userInfo->getHumanInfo(elastos::UserInfo::Item::Did, value);
    Log::I(Log::TAG, "DID       : %s", value.c_str());
    userInfo->getHumanInfo(elastos::UserInfo::Item::ElaAddress, value);
    Log::I(Log::TAG, "ElaAddress: %s", value.c_str());

    std::vector<elastos::UserInfo::CarrierInfo> carrierInfoArray;
    userInfo->getAllCarrierInfo(carrierInfoArray);
    for(auto& it: carrierInfoArray) {
        Log::I(Log::TAG, "BoundDevId  : %s", it.mDevId.c_str());
        Log::I(Log::TAG, "CarrierAddr : %s", it.mUsrAddr.c_str());
        Log::I(Log::TAG, "carrierUsrId: %s", it.mUsrId.c_str());
    }

    loop(fifoFilePath, contact);

    return 0;
}

void loop(const char* fifoFilePath, std::shared_ptr<elastos::Contact> contact)
{
    while (true) {
        if(fifoFilePath != nullptr) {
            std::string cmdLine;
            std::ifstream fifoStream(fifoFilePath, std::ifstream::in);
            std::getline(fifoStream, cmdLine);
            fifoStream.close();

            if(cmdLine.empty() == true) {
                std::this_thread::sleep_for (std::chrono::milliseconds(100));
                continue;
            }
            Log::I(Log::TAG, "==> Received Command: %s", cmdLine.c_str());

            std::string errMsg;
            int ret = ContactTestCmd::Do(contact, cmdLine, errMsg);
            if(ret < 0) {
                Log::E(Log::TAG, "ErrCode(%d): %s", ret, errMsg.c_str());
            } else {
                Log::I(Log::TAG, "Success to exec: %s", cmdLine.c_str());
            }
        } else {
            std::this_thread::sleep_for (std::chrono::seconds(1));
        }
    }
}

void signalHandler(int sig) {
    std::cerr << "Error: signal " << sig << std::endl;

    void* addrlist[512];
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
            //std::cout << __PRETTY_FUNCTION__ << " pubKey:" << pubKey << std::endl;
            return pubKey;
        };

        std::vector<uint8_t> onEncryptData(const std::string& pubKey, const std::vector<uint8_t>& src) override {
            auto dest = std::vector<uint8_t> {src.rbegin(), src.rend()};
            return dest;
        }
        std::vector<uint8_t> onDecryptData(const std::vector<uint8_t>& src) override {
            auto dest = std::vector<uint8_t> {src.rbegin(), src.rend()};
            return dest;
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

        virtual int onSigninDevicesOverflow(const std::weak_ptr<elastos::HumanInfo> userInfo, int capacity) override {
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

        virtual int onFriendRequest(elastos::FriendInfo friendInfo, std::string message) override {
            std::cout << __PRETTY_FUNCTION__ << " message:" << message << std::endl;
            return 0;
        };

        virtual void onStatusChanged(elastos::FriendInfo friendInfo, uint64_t status) override {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        };
    };

    return std::make_shared<FriendListener>();
}

std::string getPublicKey()
{
    const char* language = "english";
    const char* words = "";

    if(gCachedMnemonic.empty() == true) {
        //std::string password;
        //std::cout << "input password: ";
        //std::getline(std::cin, password);

        if(gSavedMnemonic.empty() == true) {
            gSavedMnemonic = ::generateMnemonic(language, words);
            Log::I(Log::TAG, "generate mnemonic: %s", gSavedMnemonic.c_str());
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
