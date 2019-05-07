/**
 * @file	UserInfo.hpp
 * @brief	UserInfo
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_USER_INFO_HPP_
#define _ELASTOS_USER_INFO_HPP_

#include "BaseInfo.hpp"
#include "FriendInfo.hpp"
#include "UniqueId.hpp"

namespace elastos {

class UserInfo : public BaseInfo {
public:
  /*** type define ***/
  class FriendListener {
  public:
      enum Status {
          Online,
          Offline,
      };

      explicit FriendListener() = default;
      virtual ~FriendListener() = default;

      virtual void onStatus(FriendInfo friendInfo, Status status) = 0;
      virtual int onRequest(FriendInfo friendInfo, std::string message) = 0;
      virtual void onReceiveMessage(FriendInfo friendInfo, int msgType, std::vector<int8_t> msgContent) = 0;
  };

  /*** static function and variable ***/

  /*** class function and variable ***/
  explicit UserInfo(const char* userPubKey);
  explicit UserInfo();
  virtual ~UserInfo();

  virtual void setFriendListner(FriendListener* listener);

  virtual int addFriendByDid(std::string did);
  virtual int addFriendByCarrier(std::string carrierAddress);
  virtual int addFriendByEla(std::string elaAddress);

  virtual std::string findFriend(int type, std::string content);

  virtual void sendMessage(BaseInfo friendInfo, int msgType, std::string msgContent);
  virtual void sendMessage(BaseInfo friendInfo, int msgType, std::vector<int8_t> msgContent);

  virtual void sendChainMessage(BaseInfo friendInfo, int msgType, std::vector<int8_t> msgContent,
                                const std::string& seed) = 0;

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  UniqueId mUniqueId;
  std::vector<FriendInfo> mFriendList;

  std::unique_ptr<FriendListener> mFriendListener;

}; // class UserInfo

} // namespace elastos

#endif /* _ELASTOS_USER_INFO_HPP_ */
