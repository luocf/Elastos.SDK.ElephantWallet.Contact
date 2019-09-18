import Foundation

class CrossTmplUtils {
  static func ReadTmplContent(tmplName: String) -> String {
    var tmplPath: URL
    
    do {
      tmplPath = resourcesDir!.appendingPathComponent(tmplName)
      
      let tmplContent = try String(contentsOf: tmplPath, encoding: .utf8)
      return tmplContent
    } catch {
      print("CrossTmplUtils.ReadTmplContent() Failed to read from url: \(tmplPath.path)")
      exit(1)
    }
  }

  static func WriteContent(file: URL, content: String) {
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
