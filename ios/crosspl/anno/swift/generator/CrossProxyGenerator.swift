import Foundation

open class CrossProxyGenerator {
  static func Generate(crossProxyDir: URL, classInfo: CrossClassInfo) -> Bool {
    let proxyHeaderFile = GetHeaderFile(crossProxyDir: crossProxyDir, classInfo: classInfo)
    let proxySourceFile = GetSourceFile(crossProxyDir: crossProxyDir, classInfo: classInfo)
    
    var ret = GenerateHeader(proxyFile: proxyHeaderFile, classInfo: classInfo)
    if ret == false {
      return ret
    }
    
    ret = GenerateSource(proxyFile: proxySourceFile, classInfo: classInfo)
    if ret == false {
      return ret
    }

    
    return true
  }
  
  static func GetSourceFile(crossProxyDir: URL, classInfo: CrossClassInfo) -> URL {
    return crossProxyDir.appendingPathComponent(classInfo.cppInfo.className! + ".proxy.mm")
  }
  
  static func GetHeaderFile(crossProxyDir: URL, classInfo: CrossClassInfo) -> URL {
    return crossProxyDir.appendingPathComponent(classInfo.cppInfo.className! + ".proxy.h")
  }

  private static func GenerateHeader(proxyFile: URL, classInfo: CrossClassInfo) -> Bool {
    let sourceChanged = checkSourceChanged(proxyFile: proxyFile, classInfo: classInfo)
    if sourceChanged == false {
      print("Ignore Generate: \(proxyFile.path)")
      return true
    }
    
    print("Generate: \(proxyFile.path)")
    let tmpl = CrossTmplUtils.ReadTmplContent(tmplName: CrossClassProxyHeaderTmpl)
    let content = classInfo.makeProxyDeclare(tmpl: tmpl)
    CrossTmplUtils.WriteContent(file: proxyFile, content: content)
  
    return true
  }

  private static func GenerateSource(proxyFile: URL, classInfo: CrossClassInfo) -> Bool {
    let sourceChanged = checkSourceChanged(proxyFile: proxyFile, classInfo: classInfo)
    if sourceChanged == false {
      print("Ignore Generate: \(proxyFile.path)")
      return true
    }
    
    print("Generate: \(proxyFile.path)")
    let tmpl = CrossTmplUtils.ReadTmplContent(tmplName: CrossClassProxySourceTmpl)
    let content = classInfo.makeProxySource(tmpl: tmpl)
    CrossTmplUtils.WriteContent(file: proxyFile, content: content)
  
    return true
  }

  private static func checkSourceChanged(proxyFile: URL, classInfo: CrossClassInfo) -> Bool {
    let srcAttribs = try? FileManager.default.attributesOfItem(atPath: classInfo.sourcePath!)
    let srcModifyTime = srcAttribs?[FileAttributeKey.modificationDate] as? Date
    let proxyAttribs = try? FileManager.default.attributesOfItem(atPath: proxyFile.path)
    let proxyModifyTime = proxyAttribs?[FileAttributeKey.modificationDate] as? Date
    if srcModifyTime != nil
    && proxyModifyTime != nil
    && srcModifyTime! < proxyModifyTime! { // source file not changed
      return false
    }
    
    return true
  }
  
  private static let CrossClassProxyHeaderTmpl = "/CrossPLClass.proxy.h.tmpl"
  private static let CrossClassProxySourceTmpl = "/CrossPLClass.proxy.mm.tmpl"
}
