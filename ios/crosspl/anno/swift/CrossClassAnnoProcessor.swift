import Foundation

class CrossClassAnnoProcessor {
  static func process(swiftSrcList: [String], crossplDir: URL, productName: String, bundleId: String) -> Bool {
    var classInfoList = [CrossClassInfo]()
    
    do {
      for swiftSrc in swiftSrcList {
        let swiftContent = try String(contentsOfFile: swiftSrc, encoding: .utf8)
//        print("Parse swift file: \(swiftSrc) ->\n\(swiftContent)")
        let contentLines = swiftContent.components(separatedBy: .newlines)
        for idx in contentLines.indices {
          let line = contentLines[idx]
          if(idx >= contentLines.count) {
            continue;
          }
          if(line.contains(Annotation.CrossClass) == false) {
            continue;
          }
          let classIndex = idx + 1
          let classDefine = contentLines[classIndex]
          let className = classDefine.replace(".*class (\\w*).*") { "\($0[1])" }
          print("Found \(Annotation.CrossClass): \(className)")
          
          let classInfo = CrossClassInfo.Parse(sourceFile: swiftSrc, className: className,
                                               sourceLines: contentLines, classIndex: classIndex,
                                               productName: productName, bundleId: bundleId)
          classInfoList.append(classInfo)
        }
      }
    } catch {
      print("CrossClassAnnoProcessor Error info: \(error)")
    }
    
    let crossProxyDir = crossplDir.appendingPathComponent("proxy")
    var headerFileList = [URL]()
    var sourceFileList = [URL]()
    for it in classInfoList {
      let ret = CrossProxyGenerator.Generate(crossProxyDir: crossProxyDir, classInfo: it)
      if ret == false {
        print("Failed to generate class: \(it.toString())")
        return false
      }
      
      let crossplHeaderFile = CrossProxyGenerator.GetHeaderFile(crossProxyDir: crossProxyDir, classInfo: it)
      let crossplSourceFile = CrossProxyGenerator.GetSourceFile(crossProxyDir: crossProxyDir, classInfo: it)
      headerFileList.append(crossplHeaderFile)
      sourceFileList.append(crossplSourceFile)
    }
    
    var ret = CrossPLFactoryGenerator.Generate(crossplDir: crossplDir, classInfoList: classInfoList, headerFileList: headerFileList)
    if ret == false {
      print("Failed to generate CrossPLFactory.hpp or CrossPLFactory.cpp.")
      return false
    }
//    let crossplHeaderFile = CrossPLFactoryGenerator.GetHeaderFile(crossplDir: crossplDir)
//    let crossplSourceFile = CrossPLFactoryGenerator.GetSourceFile(crossplDir: crossplDir)
//    headerFileList.append(crossplHeaderFile)
//    sourceFileList.append(crossplSourceFile)

    ret = CrossPLUtilsGenerator.Generate(crossplDir: crossplDir)
    if ret == false {
      print("Failed to generate CrossPLUtils.hpp or CrossPLUtils.cpp.")
      return false
    }
//    let crossplUtilsHeaderFile = CrossPLUtilsGenerator.GetHeaderFile(crossplDir: crossplDir)
//    let crossplUtilsSourceFile = CrossPLUtilsGenerator.GetSourceFile(crossplDir: crossplDir)
//    headerFileList.append(crossplUtilsHeaderFile)
//    sourceFileList.append(crossplUtilsSourceFile)

    
    return true
  }
}
