import Foundation

class CrossPLFactoryGenerator {
  static func Generate(crossplDir: URL, classInfoList: [CrossClassInfo], headerFileList: [URL]) -> Bool {
    let headerFile = GetHeaderFile(crossplDir: crossplDir)
    let sourceFile = GetSourceFile(crossplDir: crossplDir)
  
    var ret = GenerateHeader(headerFile, headerFileList)
    if ret == false {
      return ret
    }
  
    ret = GenerateSource(sourceFile, classInfoList, headerFileList)
    if ret == false {
      return ret
    }
  
    return true
  }
  
  static func GetSourceFile(crossplDir: URL) -> URL {
    return crossplDir.appendingPathComponent("CrossPLFactory.mm")
  }
  
  static func GetHeaderFile(crossplDir: URL) -> URL {
    return crossplDir.appendingPathComponent("CrossPLFactory.h")
  }
  
  private static func GenerateHeader(_ headerFile: URL, _ headerFileList: [URL]) -> Bool {
    print("Generate: \(headerFile.path)")
    var content = CrossTmplUtils.ReadTmplContent(tmplName: CrossPLHeaderTmpl)
  
    var includeProxyList = ""
    headerFileList.forEach { (it) in
      includeProxyList += "#import \"\(it.lastPathComponent)\"\n"
    }
    content = content.replacingOccurrences(of: TmplKeyIncludeProxyHeader, with: includeProxyList)
    
    CrossTmplUtils.WriteContent(file: headerFile, content: content)
    return true
  }
  
  private static func GenerateSource(_ sourceFile: URL, _ classInfoList: [CrossClassInfo], _ headerFileList: [URL]) -> Bool {
    print("Generate: \(sourceFile.path)")
    var content = CrossTmplUtils.ReadTmplContent(tmplName: CrossPLSourceTmpl)
  
    var includeProxyList = ""
    headerFileList.forEach { (it) in
      includeProxyList += "#import \"\(it.lastPathComponent)\"\n"
    }
    content = content.replacingOccurrences(of: TmplKeyIncludeProxyHeader, with: includeProxyList)
    
    var includeCppList = ""
    classInfoList.forEach { (it) in
      includeCppList += "#import \"\(it.cppInfo.className!).hpp\"\n"
    }
    content = content.replacingOccurrences(of: TmplKeyIncludeCppHeader, with: includeCppList)
  
    var regNativeMethodsList = ""
    classInfoList.forEach { (it) in
      regNativeMethodsList += "\(CrossTmplUtils.TabSpace)ret |= proxy::\(it.cppInfo.className!)::RegisterNativeMethods(jenv.get());\n"
    }
    content = content.replacingOccurrences(of: TmplKeyRegisterNativeMethods, with: regNativeMethodsList)
  
    var createCppObjectList = ""
    var destroyCppObjectList = ""
    classInfoList.forEach { (it) in
      createCppObjectList += "\(CrossTmplUtils.TabSpace)if(std::strcmp(swiftClassName, \"\(it.swiftInfo.className!)\") == 0) {\n"
      createCppObjectList += "\(CrossTmplUtils.TabSpace)\(CrossTmplUtils.TabSpace)ptr = new crosspl::native::\(it.swiftInfo.className!)();\n"
      createCppObjectList += "\(CrossTmplUtils.TabSpace)}\n"
    
      destroyCppObjectList += "\(CrossTmplUtils.TabSpace)if(std::strcmp(swiftClassName, \"\(it.swiftInfo.className!)\") == 0) {\n"
      destroyCppObjectList += "\(CrossTmplUtils.TabSpace)\(CrossTmplUtils.TabSpace)delete reinterpret_cast<crosspl::native::\(it.cppInfo.className!)*>(cppHandle);\n"
      destroyCppObjectList += "\(CrossTmplUtils.TabSpace)\(CrossTmplUtils.TabSpace)return 0;\n"
      destroyCppObjectList += "\(CrossTmplUtils.TabSpace)}\n"
    }
    content = content
      .replacingOccurrences(of: TmplKeyCreateCppObject, with: createCppObjectList)
      .replacingOccurrences(of: TmplKeyDestroyCppObject, with: destroyCppObjectList)
  
    content = content.replacingOccurrences(of: TmplKeyProductName, with: CrossClassInfo.ProductName!)
  
    CrossTmplUtils.WriteContent(file: sourceFile, content: content)
    return true
  }
  
  private static let CrossPLHeaderTmpl = "/CrossPLFactory.h.tmpl"
  private static let CrossPLSourceTmpl = "/CrossPLFactory.mm.tmpl"
  
  private static let TmplKeyIncludeProxyHeader = "%IncludeProxyHeader%"
  private static let TmplKeyIncludeCppHeader = "%IncludeCppHeader%"
  private static let TmplKeyRegisterNativeMethods = "%RegisterNativeMethods%"
  private static let TmplKeyCreateCppObject = "%CreateCppObject%"
  private static let TmplKeyDestroyCppObject = "%DestroyCppObject%"
  private static let TmplKeyProductName: String = "%ProductName%"

}
