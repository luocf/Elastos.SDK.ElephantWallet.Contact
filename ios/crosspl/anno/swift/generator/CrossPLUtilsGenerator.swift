import Foundation

class CrossPLUtilsGenerator {
  static func Generate(crossplDir: URL) -> Bool {
    let headerFile = GetHeaderFile(crossplDir: crossplDir)
    let sourceFile = GetSourceFile(crossplDir: crossplDir)
  
    var ret = GenerateFile(CrossPLUtilsHeaderTmpl, headerFile)
    if ret == false {
      return ret
    }
  
    ret = GenerateFile(CrossPLUtilsSourceTmpl, sourceFile)
    if ret == false {
      return ret
    }
  
    let spanFile = GetFile(crossplDir, ExperimentalSpanTmpl)
    ret = GenerateFile(ExperimentalSpanTmpl, spanFile)
    if ret == false {
      return ret
    }
  
    return true
  }
  
  static func GetSourceFile(crossplDir: URL) -> URL {
    return GetFile(crossplDir, CrossPLUtilsSourceTmpl)
  }
  
  static func GetHeaderFile(crossplDir: URL) -> URL {
    return GetFile(crossplDir, CrossPLUtilsHeaderTmpl)
  }
  
  private static func GenerateFile(_ from: String, _ to: URL) -> Bool {
    print("Generate: \(to.path)")

//    let tmplFile = CrossTmplUtils.GetResourceFile(tmplName: from)
//    let sourceNotChanged = FileManager.default.contentsEqual(atPath: tmplFile.path, andPath: to.path)
//    if sourceNotChanged == true {
//      print("Ignore Generate: \(to)")
//      return true
//    }
    
    let content = CrossTmplUtils.ReadTmplContent(tmplName: from)
    CrossTmplUtils.WriteContent(file: to, content: content)
    return true
  }
  
  private static func GetFile(_ crossplDir: URL, _ fileName: String) -> URL {
    return crossplDir.appendingPathComponent(fileName)
  }
  
  private static let CrossPLUtilsHeaderTmpl = "/CrossPLUtils.h"
  private static let CrossPLUtilsSourceTmpl = "/CrossPLUtils.mm"
  private static let ExperimentalSpanTmpl = "/experimental-span.hpp"
}
