
open class Contact: ContactBridge {
  public class Factory: ContactFactory {
    public static func Create() -> Contact {
      return Contact()
    }
  
    public class override func SetDeviceId(devId: String) {
      ContactFactory.SetDeviceId(devId: devId)
      UserInfo.setCurrDevId(devId: devId)
      
      
//   let aaa = "{\"HumanInfo\":{\"BoundCarrierArray\":[{\"CarrierAddr\":\"kpCvdLZamiUi3qECoWz8DTYzABue2HAjjEhteAh84PS2tyn5VUh\",\"CarrierId\":\"LwBD9mLbB6DsNTzJ95wCJkUXbRFXpGTBrRHBw4zsKup\",\"DeviceInfo\":{\"DevId\":\"49270136-322A-4995-87A5-78FF64E81EEA\",\"DevName\":\"Darwin\"},\"UpdateTime\":1569469125504}],\"HumanCode\":\"ioJHgvqyeFwhuEbgYzRWgPUX5eKwycprfg\",\"Status\":1,\"UpdateTime\":1569469091133,\"WalletAddressMap\":{\"ELA\":\"EZu3LwxM7rX8KAiHhodCK4YVGfrNmKX1RC\"}},\"IdentifyCode\":{\"IdCodeMap\":[]},\"IsMyself\":true}"
      
//      let aaa = "{\"HumanInfo\":{\"CommonInfoMap\":{{1,\"023c58f0ce24178a4235ee0bff80b5967e303dca9bfa41f162a2b21e1590ec7ff8\"},{2,\"ioJHgvqyeFwhuEbgYzRWgPUX5eKwycprfg\"},{3,\"EZu3LwxM7rX8KAiHhodCK4YVGfrNmKX1RC\"}},\"Status\":1,\"UpdateTime\":1569469091133}}"

      
      
      do {
//      let decode = try JSONDecoder().decode(UserInfo.UserJson.self, from: aaa.data(using: .utf8)!)
        let decode = UserInfo.UserJson()
        decode.humanInfo = HumanInfo()
        decode.humanInfo!.commonInfoMap = [:]
        decode.humanInfo!.commonInfoMap![0] = "0"
        decode.humanInfo!.commonInfoMap![1] = "1"
        print("============== \(decode.toString())")
      } catch {
        print("============== \(error)")
      }

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
