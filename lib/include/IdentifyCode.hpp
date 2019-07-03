#ifndef _ELASTOS_IDENTIFY_CODE_HPP_
#define _ELASTOS_IDENTIFY_CODE_HPP_

#include <map>
#include <string>

#include "ErrCode.hpp"

namespace elastos {

class IdentifyCode {
public:
    /*** type define ***/
    enum Type {
        PhoneNumber,
        EmailAddress,
        WechatId,
        CarrierKey,
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit IdentifyCode();
    virtual ~IdentifyCode();

    virtual int setIdentifyCode(Type type, const std::string& value);
    virtual int getIdentifyCode(Type type, std::string& value) const;

    virtual int serialize(std::string& value) const;
    virtual int deserialize(const std::string& value);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::map<std::string, std::string> mCarrierSecretKeyMap; // DevUUID: CarrierKey
    std::map<Type, std::string> mIdCodeMap; // DevUUID: CarrierId
}; // class IdentifyCode

} // namespace elastos

#endif /* _ELASTOS_IDENTIFY_CODE_HPP_ */
