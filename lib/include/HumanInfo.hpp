#ifndef _ELASTOS_HUMAN_INFO_HPP_
#define _ELASTOS_HUMAN_INFO_HPP_

#include <map>
#include <string>
#include <vector>

#include "ErrCode.hpp"

namespace elastos {

class HumanInfo {
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
    explicit HumanInfo(const std::string& did);
    virtual ~HumanInfo();

    int updateInfo(Item item, const std::string& value);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::string mChainPubKey;
    std::map<std::string, std::string> mCarrierAddrMap; // DevUUID: CarrierId

    std::string mDid;
    std::string mElaAddr;
    std::string mNickname;
    std::string mAvatar;
    std::string mGender;
    std::string mDescription;

}; // class HumanInfo

} // namespace elastos

#endif /* _ELASTOS_HUMAN_INFO_HPP_ */
