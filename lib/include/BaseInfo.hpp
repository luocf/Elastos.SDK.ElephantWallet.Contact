#ifndef _ELASTOS_BASE_INFO_HPP_
#define _ELASTOS_BASE_INFO_HPP_

#include <map>
#include <string>
#include <vector>

#include "ErrCode.hpp"

namespace elastos {

class BaseInfo {
protected:
    /*** type define ***/
    enum Item {
        Nickname = 1,
        Avatar,
        Gender,
        Description,
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit BaseInfo(const std::string& did);
    virtual ~BaseInfo();

    int updateInfo(Item item, const std::string& value);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::string mDid;
    std::string mElaAddr;
    std::map<std::string, std::string> mCarrierAddrMap; // DevUUID: CarrierId

    std::string mNickname;
    std::string mAvatar;
    std::string mGender;
    std::string mDescription;
}; // class BaseInfo

} // namespace elastos

#endif /* _ELASTOS_BASE_INFO_HPP_ */
