import CommonCrypto

public class Utils {
  private init() {}
  
  public static func getMd5(str: String) -> String {
    let length = Int(CommonCrypto.CC_MD5_DIGEST_LENGTH)
    let messageData = str.data(using: .utf8)!
    var digestData = Data(count: length)
    
    _ = digestData.withUnsafeMutableBytes { digestBytes -> UInt8 in
      messageData.withUnsafeBytes { messageBytes -> UInt8 in
        if let messageBytesBaseAddress = messageBytes.baseAddress, let digestBytesBlindMemory = digestBytes.bindMemory(to: UInt8.self).baseAddress {
          let messageLength = CC_LONG(messageData.count)
          CC_MD5(messageBytesBaseAddress, messageLength, digestBytesBlindMemory)
        }
        return 0
      }
    }
    return digestData.map { String(format: "%02hhx", $0) }.joined()
  }

}
