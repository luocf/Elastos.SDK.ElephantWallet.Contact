#ifndef _ELASTOS_FRIEND_INFO_HPP_
#define _ELASTOS_FRIEND_INFO_HPP_

#include "BaseInfo.hpp"

namespace elastos {

class FriendInfo : public BaseInfo {
public:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit FriendInfo(const char* userPubKey);
  explicit FriendInfo();
  virtual ~FriendInfo();

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  std::string mAlias;

}; // class FriendInfo

} // namespace elastos

#endif /* _ELASTOS_FRIEND_INFO_HPP_ */
