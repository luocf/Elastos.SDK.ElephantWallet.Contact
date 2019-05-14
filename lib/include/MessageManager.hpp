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
#include "FriendInfo.hpp"
#include "SecurityManager.hpp"
#include "UserInfo.hpp"

#include "MessageChannelStrategy.hpp"

namespace elastos {

class MessageManager {
public:
    /*** type define ***/
    enum class ChannelType: uint32_t {
        Carrier  = 0x000000FF,
        ElaChain = 0x00000100,
    };

    enum class MessageType: uint32_t {
        Nonspecific = 0x00000001,
        Text = 0x00000002,
        Audio = 0x00000040,
        Transfer = 0x00000080,
    };

    struct MessageInfo {
    public:
        const uint32_t mId;
        const MessageType mType;
        const std::vector<uint8_t> mPlainContent;
        const std::string mCryptoAlgorithm;
    private:
        explicit MessageInfo(MessageType type,
                             const std::vector<uint8_t>& plainContent,
                             const std::string& cryptoAlgorithm);
        virtual ~MessageInfo();

        friend MessageManager;
    };

    class MessageListener {
    public:
        explicit MessageListener() = default;
        virtual ~MessageListener() = default;

        virtual void onReceivedMessage(const FriendInfo& friendInfo,
                                       ChannelType chType,
                                       const MessageInfo& msgInfo) = 0;
        virtual void onSentMessage(int msgIndex, int errCode) = 0;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit MessageManager(std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~MessageManager();

    virtual void setMessageListener(std::shared_ptr<MessageListener> listener);

    virtual int openChannels(std::weak_ptr<Config> config);

    virtual std::shared_ptr<MessageInfo> makeMessage(MessageType type,
                                                     const std::vector<uint8_t>& plainContent,
                                                     const std::string& cryptoAlgorithm = "") const;
    virtual std::shared_ptr<MessageInfo> makeMessage(const std::string& plainContent,
                                                     const std::string& cryptoAlgorithm = "") const;

    virtual int sendMessage(const FriendInfo& friendInfo, ChannelType chType, const MessageInfo& msgInfo);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::weak_ptr<SecurityManager> mSecurityManager;
    std::shared_ptr<MessageListener> mMessageListener;
    std::weak_ptr<UserInfo> mUserInfo;

    std::map<ChannelType, std::shared_ptr<MessageChannelStrategy>> mMessageChannelMap;

}; // class MessageManager

} // namespace elastos

#endif /* _ELASTOS_MESSAGE_MANAGER_HPP_ */
