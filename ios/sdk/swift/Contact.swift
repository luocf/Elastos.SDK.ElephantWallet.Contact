
open class Contact: ContactBridge {
  public class Factory: ContactFactory {
    public static func Create() -> Contact {
      return Contact()
    }
  
    public class override func SetDeviceId(devId: String) {
      ContactFactory.SetDeviceId(devId: devId)
      //UserInfo.setCurrDevId(devId);
    }
  
    private override init() {
      super.init()
    }
  } // class Factory

  open class Listener: ContactListener {
  } // class Listener

  open class Message: ContactMessage {
  } // class Message

}
