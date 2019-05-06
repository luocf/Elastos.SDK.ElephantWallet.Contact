#ifndef _ELASTOS_CONTACT_BASE_HPP_
#define _ELASTOS_CONTACT_BASE_HPP_

namespace elastos {

class ContactBase {
protected:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit ContactBase(const char* userPubKey);
  explicit ContactBase();
  virtual ~ContactBase();

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  std::string mElaAddress;
  std::map<std:string, std::string> mCarrierIdMap; // DevUUID: CarrierId

  std::string mNickname;
  std::string mAvatar;
  std::string mGender;
  std::string mDescription;
}; // class ContactBase

} // namespace elastos

#endif /* _ELASTOS_CONTACT_BASE_HPP_ */
