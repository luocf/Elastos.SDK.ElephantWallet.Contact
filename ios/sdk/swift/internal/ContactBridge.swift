import CrossPL

/* @CrossClass */
open class ContactBridge: CrossBase {
  public static let TAG = "elastos";
  
  init() {
    super.init(className: String(describing: ContactBridge.self))
  }

  deinit {
    if(mListener != nil) {
      mListener!.unbind()
    }
  }
  
  public func setupListener(listener: Contact.Listener?) {
    if(mListener != nil) {
      mListener!.unbind();
    }
    mListener = listener;

    mListener?.bind();
    setListener(listener: mListener);
  }
//
//  public int setUserInfo(UserInfo.Item item, String value) {
//    int ret = setUserInfo(item.id(), value);
//    return ret;
//  }
//
//  public int setIdentifyCode(IdentifyCode.Type type, String value) {
//    int ret = setIdentifyCode(type.id(), value);
//    return ret;
//  }
//
//  public Contact.UserInfo getUserInfo() {
//    assert(mListener != null);
//
//    StringBuffer sbInfo = new StringBuffer();
//    int ret = getHumanInfo("-user-info-", sbInfo);
//    if(ret < 0) {
//      Log.w(TAG, "Failed to get user info. ret=" + ret);
//      return null;
//    }
//
//    Contact.UserInfo userInfo = new Contact.UserInfo();
//    ret  = userInfo.fromJson(sbInfo.toString());
//    if(ret < 0) {
//      Log.w(TAG, "Failed to deserialize user info. ret=" + ret);
//      return null;
//    }
//
//    return userInfo;
//  }
//
//  public List<Contact.FriendInfo> listFriendInfo() {
//    assert(mListener != null);
//
//    StringBuffer json = new StringBuffer();
//    int ret = getFriendList(json);
//    if(ret < 0) {
//      Log.w(TAG, "Failed to list friend info. ret=" + ret);
//      return null;
//    }
//
//    TypeToken<List<Contact.FriendInfo.FriendJson>> friendInfoListType = new TypeToken<List<Contact.FriendInfo.FriendJson>>(){};
//    List<Contact.FriendInfo.FriendJson> listJson = new Gson().fromJson(json.toString(), friendInfoListType.getType());
//
//    List<Contact.FriendInfo> list = new ArrayList<>();
//    for(Contact.FriendInfo.FriendJson it: listJson) {
//      Contact.FriendInfo friendInfo = new Contact.FriendInfo();
//      ret  = friendInfo.fromJson(it);
//      if(ret < 0) {
//        Log.w(TAG, "Failed to deserialize friend info. ret=" + ret);
//        return null;
//      }
//
//      list.add(friendInfo);
//    }
//
//    return list;
//  }
//
//  public List<String> listFriendCode() {
//    List<String> friendCodeList = new ArrayList<String>();
//
//    List<Contact.FriendInfo> friendList = listFriendInfo();
//    if(friendList == null) {
//      return friendCodeList;
//    }
//
//    for(Contact.FriendInfo it: friendList) {
//      friendCodeList.add(it.humanCode);
//    }
//
//    return friendCodeList;
//  }
//
//  public ContactStatus getStatus(String humanCode) {
//    int ret = getHumanStatus(humanCode);
//    if(ret < 0) {
//      return null;
//    }
//
//    return ContactStatus.valueOf(ret);
//  }
//
//  public Contact.Message makeMessage(ContactMessage.Type type, byte[] data, String cryptoAlgorithm) {
//    Contact.Message msg = new Contact.Message(type, data, cryptoAlgorithm);
//    return msg;
//  }
//
//  public Contact.Message makeTextMessage(String data, String cryptoAlgorithm) {
//    Contact.Message msg = new Contact.Message(ContactMessage.Type.MsgText, data.getBytes(), cryptoAlgorithm);
//    return msg;
//  }
//
//
//  public int sendMessage(String friendCode, ContactChannel channelType, Contact.Message message) {
//    if(message == null) {
//      return -1;
//    }
//
//    int ret = message.syncMessageToNative();
//    if(ret < 0) {
//      return ret;
//    }
//
//    ret = sendMessage(friendCode, channelType.id(), message);
//
//    return ret;
//  }
  
  /* @CrossNativeInterface */
  public func start() -> Int32 {
    return crosspl_Proxy_ContactBridge_start(nativeHandle)
  }
  
  /* @CrossNativeInterface */
  public func stop() -> Int32 {
    return crosspl_Proxy_ContactBridge_stop(nativeHandle)
  }
  
  /* @CrossNativeInterface */
  public func addFriend(friendCode: String, summary: String) -> Int32 {
    return crosspl_Proxy_ContactBridge_addFriend(nativeHandle, friendCode, summary)
  }
  
  /* @CrossNativeInterface */
  public func removeFriend(friendCode: String) -> Int32 {
    return crosspl_Proxy_ContactBridge_removeFriend(nativeHandle, friendCode)
  }
  
  /* @CrossNativeInterface */
  public func acceptFriend(friendCode: String) -> Int32 {
    return crosspl_Proxy_ContactBridge_acceptFriend(nativeHandle, friendCode)
  }
  
  /* @CrossNativeInterface */
  public func syncInfoDownloadFromDidChain() -> Int32 {
    return crosspl_Proxy_ContactBridge_syncInfoDownloadFromDidChain(nativeHandle)
  }
  
  /* @CrossNativeInterface */
  public func syncInfoUploadToDidChain() -> Int32 {
    return crosspl_Proxy_ContactBridge_syncInfoUploadToDidChain(nativeHandle)
  }
  
  /* @CrossNativeInterface */
  public func setWalletAddress(name: String, value: String) -> Int32 {
    return crosspl_Proxy_ContactBridge_setWalletAddress(nativeHandle, name, value)
  }
  
  /* @CrossNativeInterface */
  private func setListener(listener: CrossBase?) {
    crosspl_Proxy_ContactBridge_setListener(nativeHandle, listener)
  }
  
  /* @CrossNativeInterface */
  private func setUserInfo(item: Int32, value: String) -> Int32 {
    return crosspl_Proxy_ContactBridge_setUserInfo(nativeHandle, item, value)
  }
  
  /* @CrossNativeInterface */
  private func setIdentifyCode(item: Int32, value: String) -> Int32 {
    return crosspl_Proxy_ContactBridge_setIdentifyCode(nativeHandle, item, value)
  }
  
  /* @CrossNativeInterface */
  private func getHumanInfo(humanCode: String, info: inout String) -> Int32 {
    var nsinfo = info as NSString?
    
    let ret = crosspl_Proxy_ContactBridge_getHumanInfo(nativeHandle, humanCode, &nsinfo)
    if ret < 0 {
      return ret
    }
    info = nsinfo as! String
    
    return 0
  }
  
  /* @CrossNativeInterface */
  private func getFriendList(info: inout String) -> Int32 {
    var nsinfo = info as NSString?
    let ret = crosspl_Proxy_ContactBridge_getFriendList(nativeHandle, &nsinfo)
    if ret < 0 {
      return ret
    }
    info = nsinfo as! String
    
    return 0
  }
  
  /* @CrossNativeInterface */
  private func getHumanStatus(humanCode: String) -> Int32 {
    return crosspl_Proxy_ContactBridge_getHumanStatus(nativeHandle, humanCode)
  }
  
  /* @CrossNativeInterface */
  private func sendMessage(friendCode: String, channelType: Int32, message: CrossBase) -> Int32 {
    return crosspl_Proxy_ContactBridge_sendMessage(nativeHandle, friendCode, channelType, message)
  }
  
  private var mListener: CrossBase?
}
