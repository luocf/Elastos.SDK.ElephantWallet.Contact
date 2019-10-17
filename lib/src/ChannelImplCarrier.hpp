/**
 * @file	ChannelImplCarrier.hpp
 * @brief	ChannelImplCarrier
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_CHANNEL_IMPL_CARRIER_HPP_
#define _ELASTOS_CHANNEL_IMPL_CARRIER_HPP_

#include <Config.hpp>
#include <ela_carrier.h>
#include <ela_session.h>
#include <MessageChannelStrategy.hpp>
#include <ThreadPool.hpp>

namespace elastos {

class ChannelImplCarrier : public MessageChannelStrategy,
                                  std::enable_shared_from_this<ChannelImplCarrier> {
public:
    /*** type define ***/

    /*** static function and variable ***/
    static bool IsValidCarrierAddress(const std::string& address);
    static bool IsValidCarrierUsrId(const std::string& usrId);
    static int GetCarrierUsrIdByAddress(const std::string& address, std::string& usrId);

    /*** class function and variable ***/
    explicit ChannelImplCarrier(uint32_t chType,
                                std::shared_ptr<ChannelListener> listener,
                                std::weak_ptr<Config> config);
    virtual ~ChannelImplCarrier();

    virtual int preset(const std::string& profile) override;
    virtual int open() override;
    virtual int close() override;

    virtual int getAddress(std::string& address) override;

    virtual bool isReady() override;

    virtual int requestFriend(const std::string& friendAddr,
                              const std::string& summary,
                              bool remoteRequest = true,
                              bool forceRequest = false) override;

    virtual int removeFriend(const std::string& friendAddr) override;

    virtual int sendMessage(const std::string& friendCode,
                            std::vector<uint8_t> msgContent) override;

protected:
    /*** type define ***/

    /*** static function and variable ***/
    static void OnCarrierConnection(ElaCarrier *carrier,
                                    ElaConnectionStatus status, void *context);
    static void OnCarrierFriendRequest(ElaCarrier *carrier,
                                       const char *friendid, const ElaUserInfo *info,
                                       const char *hello, void *context);
    static void OnCarrierFriendConnection(ElaCarrier *carrier,const char *friendid,
                                          ElaConnectionStatus status, void *context);
    static void OnCarrierFriendMessage(ElaCarrier *carrier, const char *from,
                                                const void *msg, size_t len,
                                                bool offline, void *context);

    static constexpr int32_t MaxPkgSize = 1000;
    static constexpr uint8_t PkgMagic[] = { 0xA5, 0xA5, 0x5A, 0x5A,
                                            0x00/*index*/, 0x00/*index*/,
                                            0x00/*count*/, 0x00/*count*/ };
    static constexpr int32_t PkgMagicSize = 8;
    static constexpr int32_t PkgMagicHeadSize = 4;
    static constexpr int32_t PkgMagicDataIdx = 4;
    static constexpr int32_t PkgMagicDataCnt = 6;
    static constexpr int32_t MaxPkgCount = 65535; // sizeof uint16

    /*** class function and variable ***/
    void runCarrier();

    std::weak_ptr<Config> mConfig;
    std::unique_ptr<ElaCarrier, std::function<void(ElaCarrier*)>> mCarrier;
    std::unique_ptr<ThreadPool> mTaskThread;
    ChannelListener::ChannelStatus mChannelStatus;
    std::map<std::string, std::map<int, std::vector<uint8_t>>> mRecvDataCache;

}; // class ChannelImplCarrier

} // namespace elastos

#endif /* _ELASTOS_CHANNEL_IMPL_CARRIER_HPP_ */
