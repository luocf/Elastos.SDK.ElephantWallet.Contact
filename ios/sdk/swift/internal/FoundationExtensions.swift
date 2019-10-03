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

extension Encodable {
    public func toString() -> String {
        do {
          let className = String(describing: type(of: self))
          let jsonEncoder = JSONEncoder()
          let jsonData = try jsonEncoder.encode(self)
          let jsonString = String(data: jsonData, encoding: .utf8)
          return "\(className)\(jsonString!)"
        } catch {
          fatalError("Failed to call \(#function)")
        }
    }
}

public struct IntStrDictionary: Collection, Codable {
  public typealias DictionaryType = Dictionary<Int, String>
  private var dictionary: DictionaryType

  //Collection: these are the access methods
  public typealias IndexDistance = DictionaryType.IndexDistance
  public typealias Indices = DictionaryType.Indices
  public typealias Iterator = DictionaryType.Iterator
  public typealias SubSequence = DictionaryType.SubSequence

  public var startIndex: Index { return dictionary.startIndex }
  public var endIndex: DictionaryType.Index { return dictionary.endIndex }
  public subscript(position: Index) -> Iterator.Element { return dictionary[position] }
  public subscript(bounds: Range<Index>) -> SubSequence { return dictionary[bounds] }
  public var indices: Indices { return dictionary.indices }
  public subscript(key: Int) -> String? {
    get { return dictionary[key] }
    set { dictionary[key] = newValue }
  }
  public func index(after i: Index) -> Index {
    return dictionary.index(after: i)
  }

  //Sequence: iteration is implemented here
  public func makeIterator() -> DictionaryIterator<Int, String> {
    return dictionary.makeIterator()
  }

  //IndexableBase
  public typealias Index = DictionaryType.Index

  public init(from decoder: Decoder) throws {
    dictionary = DictionaryType()
    
    let container = try decoder.singleValueContainer()
    let kvMap = try container.decode([[IntOrStr]].self)
    let val = kvMap.map { kv -> (Int, String) in
      return (kv[0].int()!, kv[1].str()!)
    }
    for (k, v) in val {
      dictionary[k] = v
    }
  }
  
  public func encode(to encoder: Encoder) throws {
    var kvMap = [[IntOrStr]]()
    for (k, v) in dictionary {
      let kv = [k, v] as! [IntOrStr]
      kvMap.append(kv)
    }
    
    var container = encoder.singleValueContainer()
    try container.encode(kvMap)
  }
  
  enum IntOrStr: Codable {
    case i(Int)
    case s(String)

    init(from decoder: Decoder) throws {
      let container = try decoder.singleValueContainer()
      if let x = try? container.decode(Int.self) {
        self = .i(x)
        return
      }
      if let x = try? container.decode(String.self) {
        self = .s(x)
        return
      }
      throw DecodingError.typeMismatch(IntOrStr.self,
                                       DecodingError.Context(codingPath: decoder.codingPath,
                                                             debugDescription: "Wrong type for InnerItem"))
    }

    func encode(to encoder: Encoder) throws {
      var container = encoder.singleValueContainer()
      switch self {
      case .i(let x):
        try container.encode(x)
      case .s(let x):
        try container.encode(x)
      }
    }
    
    func int() -> Int? {
      switch self {
      case .i(let x):
        return x
      default:
        return nil
      }
    }
    func str() -> String? {
      switch self {
      case .s(let x):
        return x
      default:
        return nil
      }
    }
  }
}
