import CrossPL

/* @CrossClass */
open class ContactMessage: CrossBase {
  public enum Kind: Int {
    case Empty = 0x00000000
    case MsgText = 0x00000001
    case MsgAudio = 0x00000002
    case MsgTransfer = 0x00000004
  }
  
  public let type: Kind
  public let data: Data
  public let cryptoAlgorithm: String
  public var timestamp: Int64
  
  public func syncMessageToNative() -> Int {
    let ret = syncMessageToNative(type.rawValue, data, cryptoAlgorithm, timestamp);
    return ret;
  }
  
  init(type: Kind, data: Data, cryptoAlgorithm: String) {
    self.type = type
    self.data = data
    self.cryptoAlgorithm = cryptoAlgorithm
    self.timestamp = Int64(Date().timeIntervalSince1970 * 1000)
    
    super.init(className: String(describing: ContactMessage.self))
  }
  
  /* @CrossNativeInterface */
  private func syncMessageToNative(_ type: Int, _ data: Data, _ cryptoAlgorithm: String, _ timestamp: Int64) -> Int {
    return 0
  }
} // class Factory
