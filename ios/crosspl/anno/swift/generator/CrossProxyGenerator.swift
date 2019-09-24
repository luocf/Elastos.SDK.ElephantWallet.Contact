import Foundation

open class CrossProxyGenerator {
  static func Generate(crossProxyDir: URL, classInfo: CrossClassInfo) -> Bool {
    let proxyNativeHeaderFile = GetHeaderFile(crossProxyDir: crossProxyDir, classInfo: classInfo, isNative: true)
    let proxyPlatformHeaderFile = GetHeaderFile(crossProxyDir: crossProxyDir, classInfo: classInfo, isNative: false)
    let proxySourceFile = GetSourceFile(crossProxyDir: crossProxyDir, classInfo: classInfo)
    
    var ret = GenerateHeader(proxyFile: proxyNativeHeaderFile, classInfo: classInfo, isNative: true)
    if ret == false {
      return ret
    }
    
    ret = GenerateHeader(proxyFile: proxyPlatformHeaderFile, classInfo: classInfo, isNative: false)
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
  
  static func GetHeaderFile(crossProxyDir: URL, classInfo: CrossClassInfo, isNative: Bool) -> URL {
    return crossProxyDir.appendingPathComponent(classInfo.cppInfo.className! + (isNative ? ".native" : ".platform") + ".proxy.h")
  }

  private static func GenerateHeader(proxyFile: URL, classInfo: CrossClassInfo, isNative: Bool) -> Bool {
    print("Generate: \(proxyFile.path)")
  
    let tmpl = CrossTmplUtils.ReadTmplContent(tmplName: CrossClassProxyHeaderTmpl)
    let content = classInfo.makeProxyDeclare(tmpl: tmpl, isNative: isNative)
    CrossTmplUtils.WriteContent(file: proxyFile, content: content)
  
    return true
  }

  private static func GenerateSource(proxyFile: URL, classInfo: CrossClassInfo) -> Bool {
    print("Generate: \(proxyFile.path)")
  
    let tmpl = CrossTmplUtils.ReadTmplContent(tmplName: CrossClassProxySourceTmpl)
    let content = classInfo.makeProxySource(tmpl: tmpl)
    CrossTmplUtils.WriteContent(file: proxyFile, content: content)
  
    return true
  }

  
  private static let CrossClassProxyHeaderTmpl = "/CrossPLClass.proxy.h.tmpl"
  private static let CrossClassProxySourceTmpl = "/CrossPLClass.proxy.mm.tmpl"
}
