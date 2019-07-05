#ifndef _ELASTOS_IDENTIFY_CODE_HPP_
#define _ELASTOS_IDENTIFY_CODE_HPP_

#include <map>
#include <string>

#include "ErrCode.hpp"

namespace elastos {

class IdentifyCode {
public:
    /*** type define ***/
    enum class Type: int {
        PhoneNumber = 1,
        EmailAddress,
        WechatId,
        CarrierSecKey,
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit IdentifyCode();
    virtual ~IdentifyCode();

    virtual int setIdentifyCode(Type type, const std::string& value);
    virtual int getIdentifyCode(Type type, std::string& value) const;

    virtual int serialize(std::string& value,
                          bool withCarrierSecKey = true,
                          bool withIdCode = true) const;
    virtual int deserialize(const std::string& value);
    // virtual int print(std::string value);

    virtual int mergeIdentifyCode(const IdentifyCode& value);
private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::map<std::string, std::string> mCarrierSecretKeyMap; // DevUUID: CarrierKey
    std::map<Type, std::string> mIdCodeMap;
    long mUpdateTime;
}; // class IdentifyCode

} // namespace elastos

#endif /* _ELASTOS_IDENTIFY_CODE_HPP_ */
