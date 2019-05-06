#ifndef _ELASTOS_CONTACT_FRIEND_HPP_
#define _ELASTOS_CONTACT_FRIEND_HPP_

#include "ContactBase.hpp"
#include "ContactUniqueId.hpp"

namespace elastos {

class ContactFriend : ContactBase {
public:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit ContactFriend(const char* userPubKey);
  explicit ContactFriend();
  virtual ~ContactFriend();

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  std::string mAlias;
  ContactUniqueId mComment;

}; // class ContactFriend

} // namespace elastos

#endif /* _ELASTOS_CONTACT_FRIEND_HPP_ */
