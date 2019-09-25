import CrossPL

/* @CrossClass */
@objc open class ContactListener: CrossBase {
  open func onError(errCode: Int32, errStr: String, ext: String?) {
    fatalError("\(#function) not implementation.")
  }
  open func onAcquire(request: AcquireArgs) -> Data? {
    fatalError("\(#function) not implementation.")
  }
  //  public abstract void onEvent(EventArgs event);
  //  public abstract void onReceivedMessage(String humanCode, int channelType, Contact.Message message);
  //

//  public class EventArgs extends org.elastos.sdk.elephantwallet.contact.internal.EventArgs {
//    public EventArgs(int type, String humanCode, int channelType, byte[] data) {
//      super(type, humanCode, channelType, data);
//    }
//  }
//
//  public class StatusEvent extends EventArgs {
//    public StatusEvent(int type, String humanCode, int channelType, byte[] data) {
//      super(type, humanCode, channelType, data);
//      status = ContactStatus.valueOf(data[0]);
//    }
//    @Override
//    public String toString() {
//      return "StatusEvent" + "[type=" + type
//        + ",humanCode=" + humanCode + ",channelType=" + channelType
//        + ",status=" + status +"]";
//    }
//
//    public ContactStatus status;
//  }
//
//  public class RequestEvent extends EventArgs {
//    public RequestEvent(int type, String humanCode, int channelType, byte[] data) {
//      super(type, humanCode, channelType, data);
//      summary = new String(data);
//    }
//    @Override
//    public String toString() {
//      return "StatusEvent" + "[type=" + type
//        + ",humanCode=" + humanCode + ",channelType=" + channelType
//        + ",summary=" + summary +"]";
//    }
//
//    public String summary;
//  }
//
//  public class InfoEvent extends EventArgs {
//    public InfoEvent(int type, String humanCode, int channelType, byte[] data) {
//      super(type, humanCode, channelType, data);
//
//      String info = new String(data);
//      if(info.contains(JsonKey.IsMyself) == true) {
//        UserInfo userInfo = new UserInfo();
//        userInfo.fromJson(info);
//        humanInfo = userInfo;
//      } else if(info.contains(JsonKey.IsFriend) == true) {
//        FriendInfo friendInfo = new FriendInfo();
//        friendInfo.fromJson(info);
//        humanInfo = friendInfo;
//      } else {
//        Log.w(Contact.TAG, "InfoEvent: Failed to parse human data.");
//      }
//    }
//    @Override
//    public String toString() {
//      return "StatusEvent" + "[type=" + type
//        + ",humanCode=" + humanCode + ",channelType=" + channelType
//        + ",humanInfo=" + humanInfo +"]";
//    }
//
//    public HumanInfo humanInfo;
//  }
//
//  public ContactListener() {
//    super(ContactListener.class.getName(), 0);
//  }

  /* @CrossPlatformInterface */
  @objc internal func onAcquire(_ reqType: Int, _ pubKey: String?, _ data: Data?) -> Data? {
    Log.i(tag: Contact.TAG, msg: "ContactListener.onAcquire()")
//
    let args = AcquireArgs(type: reqType, pubKey: pubKey, data: data)
    let ret = onAcquire(request: args);

    return ret;
  }

  /* @CrossPlatformInterface */
  @objc internal func onEvent(_ eventType: Int, _ humanCode: String, _ channelType: Int, _ data: Data) {
//
//    EventArgs args = null;
//
//    EventArgs.Type type = EventArgs.Type.valueOf(eventType);
//    switch (type) {
//    case StatusChanged:
//      args = new StatusEvent(eventType, humanCode, channelType, data);
//      break;
//    case FriendRequest:
//      args = new RequestEvent(eventType, humanCode, channelType, data);
//      break;
//    case HumanInfoChanged:
//      args = new InfoEvent(eventType, humanCode, channelType, data);
//      break;
//    default:
//      throw new RuntimeException("Unimplemented type: " + type);
//    }
//
//    Log.i(Contact.TAG, "ContactListener.onEvent() args=" + args);
//    onEvent(args);
//    return;
  }

  /* @CrossPlatformInterface */
  @objc internal func onReceivedMessage(_ humanCode: String, _ channelType: Int32,
                                       _ type: Int32, _ data: Data,
                                       _ cryptoAlgorithm: String, _ timestamp: Int64) {
//    Contact.Message message = new Contact.Message(ContactMessage.Type.valueOf(type),
//                                                  data, cryptoAlgorithm);
//    message.timestamp = timestamp;
//
//    onReceivedMessage(humanCode, channelType, message);
    return;
  }
  
  /* @CrossPlatformInterface */
  @objc internal func onError(_ errCode: Int32, _ errStr: String, _ ext: String?) {
    onError(errCode: errCode, errStr: errStr, ext: ext)
  }
}
