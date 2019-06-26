#include <Elastos.SDK.Contact.hpp>
#include <BlkChnClient.hpp>
#include <iostream>
#include <Platform.hpp>
#include <signal.h>

std::string gSavedMnemonic;
std::string gCachedMnemonic;

void signalHandler(int sig);

int main(int argc, char **argv)
{
    signal(SIGSEGV, signalHandler);

    auto config = std::make_shared<elastos::Config>("/tmp/elastos.sdk.contact/test");
    std::shared_ptr<elastos::SecurityManager> sectyMgr;

    int ret = elastos::BlkChnClient::InitInstance(config, sectyMgr);
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to init instance! ret=") + std::to_string(ret));
    }

    auto bcClient = elastos::BlkChnClient::GetInstance();;

    std::map<std::string, std::string> propMap;
    propMap["key1"] = "value1";
    propMap["key2"] = "value2";
    ret = bcClient->uploadAllDidProps(propMap);
    if(ret < 0) {
        throw std::runtime_error(std::string("Failed to upload did props! ret=") + std::to_string(ret));
    }

    return 0;
}

void signalHandler(int sig) {
    std::string backtrace = elastos::Platform::GetBacktrace();
    std::cerr << backtrace << std::endl;

    exit(sig);
}

