import Foundation

class CrossClassInfo {
  static func Parse(sourceFile: String, className: String,
                    sourceLines: [String], classIndex: Int,
                    productName: String, bundleId: String) -> CrossClassInfo {
    CrossClassInfo.ProductName = productName
    
    let classInfo = CrossClassInfo()
    classInfo.cppInfo.className = className
    classInfo.swiftInfo.className = className
    classInfo.swiftInfo.classPath = bundleId + "." + className
    
    var nativeMethodList = [String]()
    var platformMethodList = [String]()
    var layer = 0;
    for idx in classIndex..<sourceLines.count {
      let line = sourceLines[idx]
      if line.contains("{") {
        layer += 1
      }
      if line.contains("}") {
        layer -= 1
        if(layer == 0) {
          break;
        }
      }
      if layer != 1 { // only recode top layer function
        continue
      }
      
      if line.contains(Annotation.CrossNativeInterface) {
        nativeMethodList.append(sourceLines[idx + 1])
      } else if line.contains(Annotation.CrossPlatformInterface) {
        platformMethodList.append(sourceLines[idx + 1])
      }
    }
//    print("nativeMethodList=\(nativeMethodList)")
//    print("platformMethodList=\(platformMethodList)")
    
    let ast = printAst(filePath: sourceFile)
    let classLines = ast.components(separatedBy: .newlines)
    var classLayer = -1
    layer = 0
    for idx in classLines.indices {
      let line = classLines[idx];
      if line.contains("{") {
        layer += 1
      } else if line.contains("}") {
        layer -= 1
      }
      if line.contains(" \(className) ") {
        classLayer = layer
      }
    
      if layer == classLayer
      && line.contains(" func ") {
        let methodName = line.replace(".*func (\\w*)\\(.*") { "\($0[1])" }
        var isCrossInterface = false
        var isNative: Bool?
        if (nativeMethodList.filter() { $0.contains("func \(methodName)(") }.count != 0) {
          isCrossInterface = true
          isNative = true
        } else if (platformMethodList.filter() { $0.contains("func \(methodName)(") }.count != 0) {
          isCrossInterface = true
          isNative = false
        }
       
        if isCrossInterface == true {
          let methodInfo = CrossMethodInfo.Parse(sourceContent: line, methodName: methodName, isNative: isNative!)
          print("  \(methodInfo.toString())")
          
          classInfo.methodInfoList.append(methodInfo)
        }
      }
    }
    
    return classInfo
  }
  
  func toString() -> String {
    var dump = String()
    dump += "ClassInfo{cppInfo=\(cppInfo.toString()),"
    dump += " swiftInfo=\(swiftInfo.toString()),"
    dump += " methodInfoList={"
    for it in methodInfoList {
      dump += it.toString() + ","
    }
    dump += "}}"
    
    return dump
  }

  
  class CppInfo {
    var className: String?
    
    func toString() -> String {
      return  "CppInfo{className=\(className ?? "nil")"
    }
  }
  
  class SwiftInfo {
    var className: String?
    var classPath: String?
    
    func toString() -> String {
      return "SwiftInfo{className=\(className ?? "nil"), classPath=\(classPath ?? "nil")"
    }
  }

  private static func printAst(filePath: String) -> String {
    let task = Process()
    task.launchPath = "/bin/bash"
    task.arguments = ["-c", "swiftc -print-ast \(filePath) 2>/dev/null"]
    
    let pipe = Pipe()
    task.standardOutput = pipe
    task.launch()
    
    let data = pipe.fileHandleForReading.readDataToEndOfFile()
    let output: String = NSString(data: data, encoding: String.Encoding.utf8.rawValue)! as String
    
    return output
  }
  
  func makeProxyDeclare(tmpl: String) -> String {
    var nativeFuncList = ""
    var platformFuncList = ""
    methodInfoList.forEach { (it) in
      let functionDeclare = it.makeProxyDeclare(cppClassName: cppInfo.className!)
      
      if it.isNative! {
        nativeFuncList += "\(functionDeclare);\n"
      } else {
        platformFuncList += "\(functionDeclare);\n"
      }
    }
  
    let content = tmpl
      .replacingOccurrences(of: CrossClassInfo.TmplKeyClassName, with: cppInfo.className!)
      .replacingOccurrences(of: CrossClassInfo.TmplKeyPlatformFunction, with: platformFuncList)
      .replacingOccurrences(of: CrossClassInfo.TmplKeyNativeFunction, with: nativeFuncList)
  
    return content
  }

  func makeProxySource(tmpl: String) -> String {
    var nativeFuncList = ""
    var platformFuncList = ""
    var objcNativeMethodList = ""
    methodInfoList.forEach { (it) in
      let funcSource = it.makeProxySource(cppClassName: cppInfo.className!, javaClassPath: swiftInfo.classPath!)
      if it.isNative! {
        nativeFuncList += "\(funcSource)"
  
        let methodContent = makeObjcNativeMethod(methodInfo: it)
        objcNativeMethodList += "\(CrossTmplUtils.TabSpace)\(CrossTmplUtils.TabSpace)\(methodContent),\n"
      } else {
        platformFuncList += "\(funcSource)"
      }
    }
  
    let content = tmpl
      .replacingOccurrences(of: CrossClassInfo.TmplKeyProductName, with: CrossClassInfo.ProductName!)
      .replacingOccurrences(of: CrossClassInfo.TmplKeyClassName, with: cppInfo.className!)
      .replacingOccurrences(of: CrossClassInfo.TmplKeyPlatformFunction, with: platformFuncList)
      .replacingOccurrences(of: CrossClassInfo.TmplKeyNativeFunction, with: nativeFuncList)
      .replacingOccurrences(of: CrossClassInfo.TmplKeyObjcNativeMethods, with: objcNativeMethodList)
      .replacingOccurrences(of: CrossClassInfo.TmplKeyObjcSwiftClass, with: swiftInfo.classPath!)
  
    return content
  }

  
  var cppInfo = CppInfo()
  var swiftInfo = SwiftInfo()
  private var methodInfoList = [CrossMethodInfo]()
  
  private func makeObjcNativeMethod(methodInfo: CrossMethodInfo) -> String {
    var funcType = "("
    methodInfo.paramsType.forEach { (it) in
      funcType += it.toSwiftString()
    }
    funcType += ")"
    funcType += methodInfo.returnType!.toSwiftString()
  
    let methodContent = "{\"\(methodInfo.methodName!)\", \"\(funcType)\", (void*)\(methodInfo.methodName!)}"
  
    return methodContent
  }

  private static var ProductName: String?
  
  private static let CrossBaseClass = "org.elastos.tools.crosspl.CrossBase"
  
  private static let TmplKeyProductName: String = "%ProductName%"
  private static let TmplKeyClassName: String = "%ClassName%"
  private static let TmplKeyPlatformFunction = "%PlatformFunction%"
  private static let TmplKeyNativeFunction = "%NativeFunction%"
  
  private static let TmplKeyObjcSwiftClass = "%ObjcSwiftClass%"
  private static let TmplKeyObjcNativeMethods = "%ObjcNativeMethods%"
}
