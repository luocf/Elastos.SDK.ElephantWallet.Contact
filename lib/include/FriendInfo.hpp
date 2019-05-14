#ifndef _ELASTOS_FRIEND_INFO_HPP_
#define _ELASTOS_FRIEND_INFO_HPP_

#include "HumanInfo.hpp"

namespace elastos {

class FriendInfo : public HumanInfo {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit FriendInfo(const std::string& did);
    explicit FriendInfo();
    virtual ~FriendInfo();

    int setAlias(const std::string& value);
    std::string getAlias() const;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::string mAlias;

}; // class FriendInfo

} // namespace elastos

#endif /* _ELASTOS_FRIEND_INFO_HPP_ */
