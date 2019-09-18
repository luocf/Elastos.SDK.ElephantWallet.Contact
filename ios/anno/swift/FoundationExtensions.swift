import Foundation

extension String {
  func replace(_ pattern: String, options: NSRegularExpression.Options = [], collector: ([String]) -> String) -> String {
    guard let regex = try? NSRegularExpression(pattern: pattern, options: options) else { return self }
    let matches = regex.matches(in: self, options: NSRegularExpression.MatchingOptions(rawValue: 0), range: NSMakeRange(0, (self as NSString).length))
    guard matches.count > 0 else { return self }
    var splitStart = startIndex
    return matches.map { (match) -> (String, [String]) in
      let range = Range(match.range, in: self)!
      let split = String(self[splitStart ..< range.lowerBound])
      splitStart = range.upperBound
      return (split, (0 ..< match.numberOfRanges)
        .compactMap { Range(match.range(at: $0), in: self) }
        .map { String(self[$0]) }
      )
      }.reduce("") { "\($0)\($1.0)\(collector($1.1))" } + self[Range(matches.last!.range, in: self)!.upperBound ..< endIndex]
  }
  func replace(_ regexPattern: String, options: NSRegularExpression.Options = [], collector: @escaping () -> String) -> String {
    return replace(regexPattern, options: options) { (_: [String]) in collector() }
  }
}

extension String {
  static func ToUnsafeMutablePointer(data: String?) -> UnsafeMutablePointer<Int8>? {
    guard data != nil else { return nil }
    
    let dataBuf = data!.data(using: String.Encoding.utf8, allowLossyConversion: false)
    let unsafePointer = UnsafeMutablePointer<UInt8>.allocate(capacity: dataBuf!.count + 1)
    unsafePointer.initialize(repeating: 0, count: dataBuf!.count + 1)
    dataBuf!.copyBytes(to: unsafePointer, count: dataBuf!.count)
    
    let opaquePtr = OpaquePointer(unsafePointer)
    
    return UnsafeMutablePointer<Int8>(opaquePtr)
  }
  static func FromUnsafeMutablePointer(data: UnsafeMutablePointer<Int8>?) -> String? {
    guard data != nil else { return nil }
    return String(cString: data!)
  }
}

extension Data {
  static func ToUnsafeMutablePointer(data: Data?) -> UnsafeMutableRawPointer? {
    guard data != nil else { return nil }
    
    let unsafePointer = UnsafeMutablePointer<UInt8>.allocate(capacity: data!.count)
    unsafePointer.initialize(repeating: 0, count: data!.count) // is this necessary?
    data!.copyBytes(to: unsafePointer, count: data!.count)
    
    let opaquePtr = OpaquePointer(unsafePointer)
    
    return UnsafeMutableRawPointer(opaquePtr)
  }
  static func FromUnsafeMutablePointer(data: UnsafeMutableRawPointer?, size: Int) -> Data? {
    guard data != nil else { return nil }
    return Data(bytes: data!, count: Int(size))
  }
}

extension Array {
  static func ToUnsafeMutablePointer(data: [Element]?) -> UnsafeMutablePointer<Element>? {
    guard data != nil else { return nil }
    
    let unsafePointer = UnsafeMutablePointer<Element>.allocate(capacity: data!.count)
    unsafePointer.initialize(from: data!, count: data!.count)
    
    let opaquePtr = OpaquePointer(unsafePointer)
    
    return UnsafeMutablePointer<Element>(opaquePtr)
  }
  static func FromUnsafeMutablePointer(data: UnsafeMutablePointer<Element>?, size: Int) -> [Element]? {
    guard data != nil else { return nil }
    return Array<Element>.FromUnsafeMutablePointer(data: data, size: size)
  }
}

extension String {
  static func ToData(from: String?) -> Data? {
    return from?.data(using: String.Encoding.utf8);
  }
}

extension Data {
  static func ToString(from: Data?) -> String? {
    guard from != nil else {
      return nil
    }
    
    return String(data: from!, encoding: String.Encoding.utf8)
  }
}
