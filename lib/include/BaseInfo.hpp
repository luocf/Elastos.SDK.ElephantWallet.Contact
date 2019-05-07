#ifndef _ELASTOS_BASE_INFO_HPP_
#define _ELASTOS_BASE_INFO_HPP_

#include <map>
#include <string>
#include <vector>

namespace elastos {

class BaseInfo {
protected:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit BaseInfo(const char* userPubKey);
  explicit BaseInfo();
  virtual ~BaseInfo();

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  std::string mElaAddress;
  std::map<std::string, std::string> mCarrierIdMap; // DevUUID: CarrierId

  std::string mNickname;
  std::string mAvatar;
  std::string mGender;
  std::string mDescription;
}; // class BaseInfo

} // namespace elastos

#endif /* _ELASTOS_BASE_INFO_HPP_ */
