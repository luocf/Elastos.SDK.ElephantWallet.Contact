#ifndef _ELASTOS_CONTACT_UNIQUE_ID_HPP_
#define _ELASTOS_CONTACT_UNIQUE_ID_HPP_

namespace elastos {

class ContactUniqueId {
public:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit ContactUniqueId(const std::string& did,
                           const std::string& phoneNumber,
                           const std::string& emailAddress,
                           const std::string& wechatId);
  explicit ContactUniqueId();
  virtual ~ContactUniqueId();

  std::string getDid();
  std::string getPhoneNumber();
  std::string getEmailAddress();
  std::string getWechatId();

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  std::string mDid;
  std::string mPhoneNumber;
  std::string mEmailAddress;
  std::string mWechatId;
}; // class ContactUniqueId

} // namespace elastos

#endif /* _ELASTOS_CONTACT_UNIQUE_ID_HPP_ */
