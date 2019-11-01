/**
 * @file	ChannelImplCarrier.hpp
 * @brief	ChannelImplCarrier
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_CHANNEL_IMPL_CARRIER_DATATRANS_HPP_
#define _ELASTOS_CHANNEL_IMPL_CARRIER_DATATRANS_HPP_

#include <sys/types.h>
#include <ela_carrier.h>
#include <ela_filetransfer.h>
#include <MessageChannelStrategy.hpp>
#include <ThreadPool.hpp>

namespace elastos {

class ChannelImplCarrierDataTrans
        : public std::enable_shared_from_this<ChannelImplCarrierDataTrans> {
public:
    /*** type define ***/
    enum Direction {
        Invalid,
        Sender,
        Receiver
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ChannelImplCarrierDataTrans(uint32_t channelType,
                                         std::shared_ptr<ElaCarrier> carrier,
                                         std::shared_ptr<MessageChannelStrategy::ChannelDataListener> listener);
    virtual ~ChannelImplCarrierDataTrans();

    int start(Direction direction, const std::string& friendCode, const std::string& dataId = "");
    int stop();

private:
    /*** type define ***/
    struct DataTransListener {
        static void OnStateChanged(ElaFileTransfer *filetransfer,
                                   FileTransferConnection state, void *context);
        static void OnCancel(ElaFileTransfer *filetransfer, const char *fileid,
                             int status, const char *reason, void *context);

    };

    struct DataSendListener {
        static void OnPull(ElaFileTransfer *filetransfer, const char *fileid,
                           uint64_t offset, void *context);

    };

    struct DataRecvListener {
        static void OnFile(ElaFileTransfer *filetransfer, const char *fileid,
                           const char *filename, uint64_t size, void *context);
        static bool OnData(ElaFileTransfer *filetransfer, const char *fileid,
                           const uint8_t *data, size_t length, void *context);
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    void runDataTransfer(const std::string fileid, uint64_t offset);
    void setDataTransStatus(MessageChannelStrategy::ChannelDataListener::Status status);

    uint32_t mChannelType;
    std::shared_ptr<ElaCarrier> mCarrier;
    std::shared_ptr<MessageChannelStrategy::ChannelDataListener> mChannelDataListener;

    std::recursive_mutex mDataTransMutex;
    ThreadPool mDataTransThread;
    ElaFileTransfer* mCarrierFileTrans;
    Direction mDataTransDirection;
    MessageChannelStrategy::ChannelDataListener::Status mDataTransStatus;
    std::string mFriendId;
    std::string mDataId;
    uint64_t mDataRecvOffset;
}; // class ChannelImplCarrier

} // namespace elastos

#endif /* _ELASTOS_CHANNEL_IMPL_CARRIER_DATATRANS_HPP_ */
