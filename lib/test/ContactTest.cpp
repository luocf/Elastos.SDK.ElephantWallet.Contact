#include <ElephantContact.hpp>
#include <Elastos.SDK.Keypair.C/Elastos.Wallet.Utility.h>
#include <memory>
#include <fstream>
#include <iostream>
#include <thread>
#include <signal.h>

#include <DateTime.hpp>
#include <Platform.hpp>
#include <Log.hpp>
#include <MD5.hpp>
#include "ContactTestCmd.hpp"
#include "P2PNetworkBase.hpp"
#include "P2PNetworkSdk.hpp"
#include "P2PNetworkBridge.hpp"
#include "MicroServiceTest.hpp"
const char* keypairLanguage = "english";
const char* keypairWords = "";

std::string gSavedMnemonic;
std::string gCachedMnemonic;

std::thread gCmdPipeMonitor;
bool gQuitFlag = false;

void loop(const char* fifoFilePath);
void signalHandler(int sig);

std::shared_ptr<elastos::MicroServiceTest> mMicroServiceTest;

int main(int argc, char **argv)
{
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);

    const char* nickname = "Friend";
    const char* fifoFilePath = (argc > 1 ? argv[1] : nullptr);
    if(fifoFilePath != nullptr) {
//        gSavedMnemonic = "bachelor sail glove swing despair lawsuit exhibit travel slot practice latin glass";
        nickname = "Me";
    } else {
        gSavedMnemonic = "month business urban nurse joy derive acquire snap venue hello city buyer";
    }
    elastos::P2PNetworkBridge& p2pNetworkBridge = elastos::P2PNetworkBridge::getInstance();
    //设置使用p2p network sdk 或者p2p network carrier
    p2pNetworkBridge.SetP2PNetwork(std::make_shared<elastos::P2PNetworkSdk>());

    mMicroServiceTest = std::make_shared<elastos::MicroServiceTest>();
    std::string publicKey = "02bc11aa5c35acda6f6f219b94742dd9a93c1d11c579f98f7e3da05ad910a48306";
    mMicroServiceTest->Create(publicKey, "/tmp/elastos.sdk.contact/test");
    mMicroServiceTest->Start();

    Log::I(Log::TAG, "Start Contact Test.");
    Log::I(Log::TAG, "%s\n", (argc > 1 ? argv[1]:""));
    gCachedMnemonic.clear();

    loop(fifoFilePath);
    return 0;
}

void processCmd(const std::string& cmdLine)
{
    /*if (cmdLine.empty() == true) {
        std::cout << "# ";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return;
    }
    Log::I(Log::TAG, "==> Received Command: %s", cmdLine.c_str());

    std::string errMsg;
    int ret = ContactTestCmd::Do(contact, cmdLine, errMsg);
    if (ret < 0) {
        Log::E(Log::TAG, "ErrCode(%d): %s", ret, errMsg.c_str());
    } else {
        Log::I(Log::TAG, "Success to exec: %s", cmdLine.c_str());
    }
    std::cout << "# ";*/
}

void monitorCmdPipe(const char* fifoFilePath) {
    if (fifoFilePath == nullptr) {
        return;
    }

    auto funcPipeMonitor = [=] {
        while (true) {
            std::string cmdLine;
            std::ifstream fifoStream(fifoFilePath, std::ifstream::in);
            std::getline(fifoStream, cmdLine);
            fifoStream.close();

            if (cmdLine == "q" || cmdLine == "quit") {
                gQuitFlag = true;
                return;
            }
            //processCmd(cmdLine, contact);
        };
    };

    gCmdPipeMonitor = std::thread (funcPipeMonitor);
}

void loop(const char* fifoFilePath)
{
    monitorCmdPipe(fifoFilePath);

    while (true) {
        std::string cmdLine;
        std::getline(std::cin, cmdLine);

        if (cmdLine == "q" || cmdLine == "quit") {
            gQuitFlag = true;
            return;
        }
        //processCmd(cmdLine, contact);
    }
}

void signalHandler(int sig) {
    std::cerr << "Error: signal " << sig << std::endl;

    std::string backtrace = elastos::Platform::GetBacktrace();
    std::cerr << backtrace << std::endl;

    exit(sig);
}
