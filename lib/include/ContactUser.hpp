/**
 * @file	ContactUser.hpp
 * @brief	ContactUser
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_CONTACT_USER_HPP_
#define _ELASTOS_CONTACT_USER_HPP_

#include "ContactBase.hpp"
#include "ContactUniqueId.hpp"

namespace elastos {

class ContactUser : ContactBase {
public:
  /*** type define ***/
  class FriendListener {
      virtual void onStatus(ContactFriend friend, Status status) = 0;
      virtual int onRequest(FriendInfo friend, std::string message) = 0;
      virtual void onReceiveMessage(BaseInfo friend, int msgType, byte[] msgContent) = 0;
  };

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit ContactUser(const char* userPubKey);
  explicit ContactUser();
  virtual ~ContactUser();

  void setFriendListner(FriendListener* listener);

  int AddFriendByDid(std::string did);
  int AddFriendByCarrier(std::string carrierAddress);
  int AddFriendByEla(std::string elaAddress);

  std::string findFriend(int type, std::string content);


private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  ContactUniqueId mUniqueId;
  std::vector<ContactFriend> mFriendList;

  FriendListener mFriendListener;

}; // class ContactUser

} // namespace elastos

#endif /* _ELASTOS_CONTACT_USER_HPP_ */
