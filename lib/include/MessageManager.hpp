/**
 * @file	MessageManager.hpp
 * @brief	MessageManager
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_MESSAGE_MANAGER_HPP_
#define _ELASTOS_MESSAGE_MANAGER_HPP_

#include "Config.hpp"
#include "ErrCode.hpp"
#include "HumanInfo.hpp"
#include "SecurityManager.hpp"
#include "UserManager.hpp"
#include "FriendManager.hpp"

#include "MessageChannelStrategy.hpp"

namespace elastos {

class MessageManager : public std::enable_shared_from_this<MessageManager> {
public:
    /*** type define ***/
    enum class ChannelType: int {
        Carrier = 1,
        ElaChain = 2,
        Email = 3,
    };

    enum class MessageType: uint32_t {
        Empty = 0x00000000,

        Message = 0x000000FF,
        MsgText = 0x00000001,
        MsgAudio = 0x00000002,
        MsgTransfer = 0x00000004,

        Control = 0x00FF0000,
        CtrlSyncDesc = 0x00010000,
    };

    struct MessageInfo {
    public:
        MessageType mType;
        std::vector<uint8_t> mPlainContent;
        std::string mCryptoAlgorithm;
        uint64_t mTimeStamp;
    private:
        explicit MessageInfo(MessageType type,
                             const std::vector<uint8_t>& plainContent,
                             const std::string& cryptoAlgorithm);
        explicit MessageInfo(const MessageInfo& info,
                             bool ignoreContent = true);
        explicit MessageInfo();
        virtual ~MessageInfo();

        friend MessageManager;
    };

    class MessageListener : public MessageChannelStrategy::ChannelListener {
    public:
        explicit MessageListener() = default;
        virtual ~MessageListener() = default;

        virtual void onStatusChanged(std::shared_ptr<UserInfo> userInfo,
                                     ChannelType channelType,
                                     UserInfo::Status status) = 0;

        virtual void onReceivedMessage(std::shared_ptr<HumanInfo> humanInfo,
                                       ChannelType channelType,
                                       const std::shared_ptr<MessageInfo> msgInfo) = 0;

        virtual void onSentMessage(int msgIndex, int errCode) override = 0;

        virtual void onFriendRequest(std::shared_ptr<FriendInfo> friendInfo,
                                     ChannelType channelType,
                                     const std::string& summary) = 0;

        virtual void onFriendStatusChanged(std::shared_ptr<FriendInfo> friendInfo,
                                           ChannelType channelType,
                                           FriendInfo::Status status) = 0;

        virtual void onHumanInfoChanged(std::shared_ptr<HumanInfo> humanInfo,
                                        ChannelType channelType) = 0;

    private:
        virtual void onStatusChanged(const std::string& userCode,
                                     uint32_t channelType,
                                     ChannelStatus status) override;

        virtual void onReceivedMessage(const std::string& friendCode,
                                       uint32_t channelType,
                                       const std::vector<uint8_t>& msgContent) override;

        //virtual void onSentMessage(int msgIndex, int errCode) override;

        virtual void onFriendRequest(const std::string& friendCode,
                                     uint32_t channelType,
                                     const std::string& summary) override;

        virtual void onFriendStatusChanged(const std::string& friendCode,
                                           uint32_t channelType,
                                           ChannelStatus status) override;

        std::weak_ptr<MessageManager> mMessageManager;
        friend class MessageManager;
    };

    /*** static function and variable ***/
    static std::shared_ptr<MessageInfo> MakeEmptyMessage();

    static std::shared_ptr<MessageInfo> MakeMessage(MessageType type,
                                                    const std::vector<uint8_t>& plainContent,
                                                    const std::string& cryptoAlgorithm = "");
    static std::shared_ptr<MessageInfo> MakeTextMessage(const std::string& plainContent,
                                                        const std::string& cryptoAlgorithm = "");


    /*** class function and variable ***/
    explicit MessageManager(std::weak_ptr<SecurityManager> sectyMgr,
                            std::weak_ptr<UserManager> userMgr,
                            std::weak_ptr<FriendManager> friendMgr);
    virtual ~MessageManager();

    virtual void setMessageListener(std::shared_ptr<MessageListener> listener);

    virtual int presetChannels(std::weak_ptr<Config> config);
    virtual int openChannels();
    virtual int closehannels();

    virtual int requestFriend(const std::string& friendAddr,
                              ChannelType chType,
                              const std::string& summary,
                              bool remoteRequest = true,
                              bool forceRequest = false);

    virtual int updateFriend(const std::string& did);

    virtual int removeFriend(const std::string& friendCode, ChannelType humanChType);

//    virtual int monitorDidChainCarrierID(const std::string& did);

    virtual int sendMessage(const std::shared_ptr<HumanInfo> humanInfo,
                            ChannelType chType,
                            const std::shared_ptr<MessageInfo> msgInfo,
                            bool sendToOtherDev = true);

    virtual int broadcastDesc(ChannelType chType);

private:
    /*** type define ***/

    /*** static function and variable ***/
    static std::shared_ptr<MessageInfo> MakeMessage(std::shared_ptr<MessageInfo> from, bool ignoreContent = true);

    /*** class function and variable ***/
    template <class T>
    int getChannel(ChannelType chType, std::weak_ptr<T>& channel);
    int sendDescMessage(const std::vector<std::shared_ptr<HumanInfo>>& humanList, ChannelType chType);

    std::weak_ptr<SecurityManager> mSecurityManager;
    std::weak_ptr<UserManager> mUserManager;
    std::weak_ptr<FriendManager> mFriendManager;
    std::shared_ptr<MessageListener> mMessageListener;

    std::map<ChannelType, std::shared_ptr<MessageChannelStrategy>> mMessageChannelMap;

    friend class UserManager;
}; // class MessageManager

/***********************************************/
/***** class template function implement *******/
/***********************************************/
template <class T>
int MessageManager::getChannel(ChannelType chType, std::weak_ptr<T>& channel)
{
    const auto& it = mMessageChannelMap.find(chType);
    if(it == mMessageChannelMap.end()) {
        return ErrCode::NotFoundError;
    }

    auto ptr = std::dynamic_pointer_cast<T>(it->second);
    if(ptr.get() == nullptr) {
        return ErrCode::InvalidArgument;
    }

    channel = ptr;

    return 0;
}

} // namespace elastos

#endif /* _ELASTOS_MESSAGE_MANAGER_HPP_ */
