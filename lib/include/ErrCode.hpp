#ifndef _ELASTOS_ERRCODE_HPP_
#define _ELASTOS_ERRCODE_HPP_

#include <string>

namespace elastos {

class ErrCode {
public:
    /*** type define ***/

    /*** static function and variable ***/
    constexpr static const int UnknownError = -1;
    constexpr static const int NotFoundError = -2;
    constexpr static const int NotReadyError = -3;
    constexpr static const int InvalidArgument = -4;
    constexpr static const int IOSystemException = -5;
    constexpr static const int NetworkException = -6;
    constexpr static const int PointerReleasedError = -7;
    constexpr static const int DevUUIDError = -8;
    constexpr static const int FileNotExistsError = -9;
    constexpr static const int JsonParseException = -10;
    constexpr static const int ConflictWithExpectedError = -11;
    constexpr static const int MergeInfoFailed = -12;

    constexpr static const int InvalidLocalDataDir = -50;
    constexpr static const int NoSecurityListener = -51;
    constexpr static const int BadSecurityValue = -52;
    constexpr static const int KeypairError = -53;

    constexpr static const int ChannelFailedPresetAll = -200;
    constexpr static const int ChannelFailedOpenAll = -201;
    constexpr static const int ChannelFailedMultiOpen = -202;
    constexpr static const int ChannelFailedCarrier = -203;
    constexpr static const int ChannelNotFound = -207;
    constexpr static const int ChannelNotReady = -208;
    constexpr static const int ChannelNotEstablished = -209;
    constexpr static const int ChannelNotOnline = -210;
    constexpr static const int ChannelNotSendMessage = -211;
    constexpr static const int ChannelDataTooLarge = -212;

    constexpr static const int BlkChnSetPropError = -300;

    constexpr static const int HttpClientError = -500;

    constexpr static const int StdSystemErrorIndex = -1000;

    static std::string ToString(int errCode);

    /*** class function and variable ***/

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ErrCode() = delete;
    virtual ~ErrCode() = delete;

}; // class ErrCode

} // namespace elastos

#endif /* _ELASTOS_ERRCODE_HPP_ */
