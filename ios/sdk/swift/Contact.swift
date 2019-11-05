
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
  
  open class DataListener: ContactDataListener {
  } // class Listener

  public class UserInfo: ContactSDK.UserInfo {
  } // class UserInfo

  public class FriendInfo: ContactSDK.FriendInfo {
  } // class FriendInfo
  
  public class Message: ContactMessage {
  } // class Message

  public class Debug: ContactDebug {
  } // class Listener
}
