#include <ContactTestCmd.hpp>

#include <iostream>
#include <iterator>
#include <sstream>

#include <DidChnClient.hpp>

/* =========================================== */
/* === static variables initialize =========== */
/* =========================================== */
const std::vector<ContactTestCmd::CommandInfo> ContactTestCmd::gCommandInfoList{
    { 'q', "quit",             nullptr,                         "\t\tQuit." },
    { 'h', "help",             ContactTestCmd::Help,            "\t\tPrint help usages." },
    { 'p', "print-info",       ContactTestCmd::PrintInfo,       "\tPrint current contact infos." },
    { 'c', "print-carrier",    ContactTestCmd::PrintCarrier,    "\tPrint current carrier infos." },
    { 'd', "print-cachedinfo", ContactTestCmd::PrintCachedInfo, "Print current carrier infos." },
    { 'u', "upload-info",      ContactTestCmd::UploadInfo,      "\tUpload info to did chain." },
    { 'a', "add-friend",       ContactTestCmd::AddFriend,       "\tAdd a friend by [did, ela address or carrier address]." },
    { 's', "send-message",     ContactTestCmd::SendMessage,     "\tSend message to a friend like: s [friendCode] [chType(1 or 2)] [msg]" },
};

/* =========================================== */
/* === static function implement ============= */
/* =========================================== */
int ContactTestCmd::Do(std::shared_ptr<elastos::Contact> contact,
                       const std::string& cmdLine,
                       std::string& errMsg)
{
    auto wsfront=std::find_if_not(cmdLine.begin(), cmdLine.end(),
                                 [](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(cmdLine.rbegin(), cmdLine.rend(),
                                 [](int c){return std::isspace(c);}).base();
    auto trimCmdLine = (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));

    std::istringstream iss(trimCmdLine);
    std::vector<std::string> args {std::istream_iterator<std::string>{iss},
                                   std::istream_iterator<std::string>{}};
    if (args.size() <= 0) {
        return 0;
    }
    const auto& cmd = args[0];

    for(const auto& cmdInfo : gCommandInfoList) {
        if(cmd.compare(0, 1, &cmdInfo.mCmd) != 0
        && cmd != cmdInfo.mLongCmd) {
            continue;
        }

        int ret = cmdInfo.mFunc(contact, args, errMsg);
        return ret;
    }

    errMsg = "Unknown command: " + cmd;
    return -10000;
}

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */
int ContactTestCmd::Help(std::shared_ptr<elastos::Contact> contact,
                         const std::vector<std::string>& args,
                         std::string& errMsg)
{
    std::cout << "Usage:" << std::endl;
    for(const auto& cmdInfo : gCommandInfoList) {
        std::cout << "  " << cmdInfo.mCmd << " | " << cmdInfo.mLongCmd << ": " << cmdInfo.mUsage << std::endl;
    }
    std::cout << std::endl;

    return 0;
}

int ContactTestCmd::PrintInfo(std::shared_ptr<elastos::Contact> contact,
                              const std::vector<std::string>& args,
                              std::string& errMsg)
{
    std::string value;

    auto weakUserMgr = contact->getUserManager();
    auto userMgr = weakUserMgr.lock();
    userMgr->serialize(value);
    std::cout << "==========================" << std::endl;
    std::cout << "UserInfo:" << std::endl;
    std::cout << value << std::endl;

    auto weakFriendMgr = contact->getFriendManager();
    auto friendMgr = weakFriendMgr.lock();
    friendMgr->serialize(value);
    std::cout << "FriendInfo:" << std::endl;
    std::cout << value << std::endl;


    return 0;
}

int ContactTestCmd::PrintCarrier(std::shared_ptr<elastos::Contact> contact,
                                 const std::vector<std::string>& args,
                                 std::string& errMsg)
{
    std::string value;

    std::cout << "==========================" << std::endl;
    auto weakUserMgr = contact->getUserManager();
    std::shared_ptr<elastos::UserInfo> userInfo;
    weakUserMgr.lock()->getUserInfo(userInfo);

    std::vector<elastos::HumanInfo::CarrierInfo> carrierInfoArray;
    userInfo->getAllCarrierInfo(carrierInfoArray);
    std::cout << "UserCarrierInfo:" << std::endl;
    for(auto& it: carrierInfoArray) {
        std::cout << " addr:" << it.mUsrAddr << ", id:" << it.mUsrId  << std::endl;
    }


    auto weakFriendMgr = contact->getFriendManager();
    std::vector<std::shared_ptr<elastos::FriendInfo>> friendList;
    weakFriendMgr.lock()->getFriendInfoList(friendList);
    for(auto& friendInfo: friendList) {
        std::cout << "FriendCarrierInfo:" << std::endl;
        std::vector<elastos::HumanInfo::CarrierInfo> carrierInfoArray;
        friendInfo->getAllCarrierInfo(carrierInfoArray);
        for(auto& it: carrierInfoArray) {
            std::cout << " addr:" << it.mUsrAddr << ", id:" << it.mUsrId  << std::endl;
        }
    }

    return 0;
}

int ContactTestCmd::PrintCachedInfo(std::shared_ptr<elastos::Contact> contact,
                                    const std::vector<std::string>& args,
                                    std::string& errMsg)
{
    auto dcClient = elastos::DidChnClient::GetInstance();

    std::string value;
    dcClient->printCachedDidProp(value);

    std::cout << "==========================" << std::endl;
    std::cout << value << std::endl;
    std::cout << std::endl;

    return 0;
}

int ContactTestCmd::UploadInfo(std::shared_ptr<elastos::Contact> contact,
                               const std::vector<std::string>& args,
                               std::string& errMsg)
{
    int ret = contact->syncInfoUploadToDidChain();
    if (ret < 0) {
      errMsg = "Failed to upload info. ret=" + std::to_string(ret);
      return -1;
    }

    return 0;
}

int ContactTestCmd::AddFriend(std::shared_ptr<elastos::Contact> contact,
                              const std::vector<std::string>& args,
                              std::string& errMsg)
{
    if(args.size() < 2) {
        errMsg = "Bad input count: " + std::to_string(args.size());
        return -1;
    }

    auto weakFriendMgr = contact->getFriendManager();
    auto friendMgr = weakFriendMgr.lock();
    if(friendMgr.get() == nullptr) {
        errMsg = "FriendManager has been released.";
        return -1;
    }

    auto friendId = args.size() > 1 ? args[1] : "";
    auto summary = args.size() > 2 ? args[2] : "";

    int ret = friendMgr->tryAddFriend(friendId, summary);
    if(ret < 0) {
        errMsg = "Failed to add friend ret=" + std::to_string(ret);
        return ret;
    }

    return 0;
}


int ContactTestCmd::SendMessage(std::shared_ptr<elastos::Contact> contact,
                                const std::vector<std::string>& args,
                                std::string& errMsg)
{
    if(args.size() < 4) {
        errMsg = "Bad input";
        return -1;
    }

    auto friendCode = args.size() > 1 ? args[1] : "";
    auto channelType = args.size() > 2 ? args[2] : "";
    auto msg = args.size() > 3 ? args[3] : "";
    if(channelType.empty() == true) {
        errMsg = "Channel Type not exists";
        return -1;
    }

    auto weakFriendMgr = contact->getFriendManager();
    auto friendMgr = weakFriendMgr.lock();
    if(friendMgr.get() == nullptr) {
        errMsg = "FriendManager has been released.";
        return -1;
    }

    std::shared_ptr<elastos::FriendInfo> friendInfo;
    int ret = friendMgr->tryGetFriendInfo(friendCode, friendInfo);
    if(ret < 0) {
        errMsg = "Failed to find friend ret=" + std::to_string(ret);
        return ret;
    }

    auto weakMsgMgr = contact->getMessageManager();
    auto msgMgr = weakMsgMgr.lock();
    if(msgMgr.get() == nullptr) {
        errMsg = "msgManager has been released.";
        return -1;
    }

    int chType = std::stoi(channelType);
    auto msgInfo = elastos::MessageManager::MakeTextMessage(msg);

    ret = msgMgr->sendMessage(friendInfo, static_cast<elastos::MessageManager::ChannelType>(chType), msgInfo);
    if(ret < 0) {
        errMsg = "Failed to send message ret=" + std::to_string(ret);
        return ret;
    }

    return 0;
}
