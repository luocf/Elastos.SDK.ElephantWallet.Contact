
open class Contact: ContactBridge {
  public class Factory: ContactFactory {
    public static func Create() -> Contact {
      return Contact()
    }
  
    public class override func SetDeviceId(devId: String) {
      ContactFactory.SetDeviceId(devId: devId)
      UserInfo.setCurrDevId(devId: devId)
    }
    
    private override init() {
      super.init()
    }
  } // class Factory

  open class Listener: ContactListener {
  } // class Listener

  public class UserInfo: ContactSDK.UserInfo {
  } // class UserInfo

  public class FriendInfo: ContactSDK.FriendInfo {
  } // class FriendInfo
  
  public class Message: ContactMessage {
  } // class Message

//  public abstract static class Debug extends ContactDebug {
//  } // class Listener
}

//extension Dictionary {
//    public init(keyValuePairs: [(Int, Value)]) {
//        self.init()
//        for pair in keyValuePairs {
//            self[pair.0] = pair.1
//        }
//    }
//}
