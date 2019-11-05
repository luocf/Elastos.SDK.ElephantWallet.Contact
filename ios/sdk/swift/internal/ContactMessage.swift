import CrossPL

/* @CrossClass */
open class ContactMessage: CrossBase {
  public enum Kind: Int {
    case Empty       = 0x00000000
    case MsgText     = 0x00000001
    case MsgAudio    = 0x00000002
    case MsgTransfer = 0x00000004
    case MsgImage    = 0x00000008
    case MsgFile     = 0x00000010
  }
  
  public class MsgData: Codable {
    public func toString() -> String {
      let encode = try! JSONEncoder().encode(self)
      let val = String(data: encode, encoding: .utf8)!
      return val
    }
    public func toData() -> Data {
      return String.ToData(from: self.toString())!
    }
  }

  public class TextData: MsgData {
    public init(text: String) {
      self.text = text;
      super.init()
    }
    
    required init(from decoder: Decoder) throws {
      fatalError("init(from:) has not been implemented")
    }
    
    public private(set) var text: String
    
    enum CodingKeys : String, CodingKey {
      case text
      var rawValue: String {
        get {
          switch self {
            case .text: return JsonKey.Text
          }
        }
      }
    }
  }

  public class FileData: MsgData {
    public init(file: URL) {
      devId = UserInfo.GetCurrDevId()
      name = file.lastPathComponent
      size = try? FileManager.default.attributesOfItem(atPath: file.path)[FileAttributeKey.size] as? Int64
      md5 = Utils.getMD5Sum(file: file)
      super.init()
    }
    
    required init(from decoder: Decoder) throws {
      fatalError("init(from:) has not been implemented")
    }
    
    public private(set) var devId: String?
    public private(set) var name: String?
    public private(set) var size: Int64?
    public private(set) var md5: String?
    
    enum CodingKeys : String, CodingKey {
      case devId
      case name
      case size
      case md5
      var rawValue: String {
        get {
          switch self {
            case .devId: return JsonKey.DeviceId
            case .name: return JsonKey.Name
            case .size: return JsonKey.Size
            case .md5: return JsonKey.Md5
          }
        }
      }
    }
  }

  
  public let type: Kind
  public private(set) var data: MsgData?
  public let cryptoAlgorithm: String
  public var timestamp: Int64
  
  public func syncMessageToNative() -> Int {
    let ret = syncMessageToNative(type: type.rawValue,
                                  data: data!.toData(),
                                  cryptoAlgorithm: cryptoAlgorithm,
                                  timestamp: timestamp)
    return ret;
  }
  
  public init(type: Kind, data: MsgData?, cryptoAlgorithm: String?) {
    self.type = type
    self.data = data
    self.cryptoAlgorithm = cryptoAlgorithm ?? ""
    self.timestamp = Int64(Date().timeIntervalSince1970 * 1000)
    
    super.init(className: String(describing: ContactMessage.self))
  }
  
  public convenience init(type: Kind, data: Data?, cryptoAlgorithm: String?) {
    self.init(type: type, data: nil as MsgData?, cryptoAlgorithm: cryptoAlgorithm)
      if(data != nil) {
        self.data = try! JSONDecoder().decode(ContactMessage.GetDataClass(type: type)!,
                                               from: data!)
      }
  }
  
  static private func GetDataClass(type: Kind) -> MsgData.Type? {
      switch (type) {
      case .MsgText:
        return TextData.self
      case .MsgFile:
        return FileData.self
      default:
        return nil
      }
  }
  
  /* @CrossNativeInterface */
  private func syncMessageToNative(type: Int,
                                   data: Data, cryptoAlgorithm: String,
                                   timestamp: Int64) -> Int {
    let ret = crosspl_Proxy_ContactMessage_syncMessageToNative(nativeHandle, Int32(type), data, cryptoAlgorithm, timestamp)
    return Int(ret)
  }
}
