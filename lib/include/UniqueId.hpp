#ifndef _ELASTOS_UNIQUE_ID_HPP_
#define _ELASTOS_UNIQUE_ID_HPP_

namespace elastos {

class UniqueId {
public:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit UniqueId(const std::string& did,
                           const std::string& phoneNumber,
                           const std::string& emailAddress,
                           const std::string& wechatId);
  explicit UniqueId();
  virtual ~UniqueId();

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
}; // class UniqueId

} // namespace elastos

#endif /* _ELASTOS_UNIQUE_ID_HPP_ */
