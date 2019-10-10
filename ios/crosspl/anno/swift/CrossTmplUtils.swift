import Foundation

class CrossTmplUtils {
  static func GetResourceFile(tmplName: String) -> URL {
    let tmplPath = resourcesDir!.appendingPathComponent(tmplName)
    return tmplPath
  }
  
  static func ReadTmplContent(tmplName: String) -> String {
    var tmplPath: URL
    
    do {
      tmplPath = GetResourceFile(tmplName: tmplName)
      
      let tmplContent = try String(contentsOf: tmplPath, encoding: .utf8)
      return tmplContent
    } catch {
      print("CrossTmplUtils.ReadTmplContent() Failed to read from url: \(tmplPath.path)")
      exit(1)
    }
  }

  static func WriteContent(file: URL, content: String, forceWrite: Bool = false) {
    if forceWrite == false {
      let tmplContent = try? String(contentsOf: file, encoding: .utf8)
      if tmplContent != nil
      && tmplContent! == content {
//        print("========== tmplContent:\(tmplContent!)")
//        print("========== content:\(content)")
        print("CrossTmplUtils.WriteContent() Ignore to write to url: \(file.path)")
        return
      }
    }
    
    do {
      try content.write(to: file, atomically: false, encoding: .utf8)
    } catch {
      print("CrossTmplUtils.WriteContent() Failed to write to url: \(file.path)")
      exit(1)
    }
  }
  
  static var resourcesDir: URL?
  
  static let TabSpace = "  "
}
