import Foundation

class CrossMethodInfo {
  static func Parse(sourceContent: String, methodName: String, isNative: Bool) -> CrossMethodInfo {
    let methodInfo = CrossMethodInfo()
//    print("CrossMethodInfo.Parse() ============== 0")
    methodInfo.methodName = methodName
    
    let paramsContent = sourceContent.replace(".*\\((.*)\\).*") { "\($0[1])" }
    let paramsLines = paramsContent.components(separatedBy: ",")
    for line in paramsLines {
      if line.isEmpty == true {
        continue
      }
      let typeLines = line.components(separatedBy: ":")
      let type = CrossVariableType.Parse(sourceContent: typeLines[1])
      
      methodInfo.paramsType.append(type)
    }
    
//    print("CrossMethodInfo.Parse() ============== 1 -\(sourceContent)-")
    var returnContent = sourceContent.replace(".*\\)(.*)") { "\($0[1])" }
//    print("CrossMethodInfo.Parse() ============== 2 -\(returnContent)-")
    if returnContent.isEmpty == true {
      returnContent = "Void"
    } else {
      returnContent = returnContent.replacingOccurrences(of: "->", with: "")
    }
    let returnType = CrossVariableType.Parse(sourceContent: returnContent)
    methodInfo.returnType = returnType
    
    methodInfo.isStatic = sourceContent.contains(" static ")
    methodInfo.isNative = isNative
//    print("CrossMethodInfo.Parse() ============== 4")
    return methodInfo
  }
  
  func makeProxyDeclare(cppClassName: String) -> String {
    if self.isNative! == true {
      return makeNativeFunctionDeclare(cppClassName: cppClassName)
    } else {
      return makePlatformFunctionDeclare(cppClassName: cppClassName)
    }
  }

  func makeProxySource(cppClassName: String, javaClassPath: String) -> String {
    var funcDeclare: String
    if self.isNative! == true {
      funcDeclare = makeNativeFunctionDeclare(cppClassName: cppClassName)
    } else {
      funcDeclare = makePlatformFunctionDeclare(cppClassName: cppClassName)
    }
    let emptyFunc = "\(funcDeclare)\n{\n\(CrossMethodInfo.TmplKeyFuncBody)\n}\n"
  
    
    var funcBody: String
    if self.isNative! == true {
      funcBody = makeNativeFunctionBody(cppClassName: cppClassName)
    } else {
      funcBody = makePlatformFunctionBody(cppClassName: cppClassName, javaClassPath: javaClassPath)
    }
  
    let content = emptyFunc.replacingOccurrences(of: CrossMethodInfo.TmplKeyFuncBody, with: funcBody)
  
    return content
  }

  
  func toString() -> String {
    var dump = String()
    dump += "MethodInfo{methodName=\(methodName!),"
    dump += " params={"
    for t in paramsType {
      dump += t.toString() + ","
    }
    dump += "},"
    dump += " returnType=\(returnType!.toString()),"
    dump += " static=\(isStatic!), native=\(isNative!)}"
    
    return dump
  }

  
  var methodName: String?
  var paramsType = [CrossVariableType]()
  var returnType: CrossVariableType?
  var isStatic: Bool?
  var isNative: Bool?
  
  private func makeNativeFunctionDeclare(cppClassName: String) -> String {
    let returnType = self.returnType!.toObjcString(isConst: false)
    var content = "\(returnType) crosspl_Proxy_\(cppClassName)_\(self.methodName!)(\(CrossMethodInfo.TmplKeyArguments))"
    
    var arguments = ""
    if isStatic == false {
      arguments += "int64_t nativeHandle, "
    }

    for idx in 0..<paramsType.count {
      let type = paramsType[idx].toObjcString()
      arguments += "\(type) ocvar\(idx), "
    }
    if arguments.isEmpty == false {
      let endIndex = arguments.index(arguments.endIndex, offsetBy: -2)
      arguments = String(arguments[..<endIndex])
    }
    content = content.replacingOccurrences(of: CrossMethodInfo.TmplKeyArguments, with: arguments)
    
    return content
  }
  
  private func makePlatformFunctionDeclare(cppClassName: String) -> String {
    var retTypeStr = self.returnType!.toCppString(isConst: false)//.removeSuffix("*")
    if returnType!.type! == .CROSSBASE {
      retTypeStr = "\(retTypeStr)*"
    } else if self.returnType!.isPrimitiveType() == false {
      retTypeStr = "std::shared_ptr<\(retTypeStr)>"
    }
    var content = "\(retTypeStr) crosspl_Proxy_\(cppClassName)_\(self.methodName!)(\(CrossMethodInfo.TmplKeyArguments))"
    
    var arguments = ""
    if isStatic == false {
      arguments += "int64_t platformHandle, "
    }
    for idx in 0..<paramsType.count {
      let type = paramsType[idx].toCppString(isConst: true)
      arguments += "\(type) var\(idx), "
    }
    if arguments.isEmpty == false {
      let endIndex = arguments.index(arguments.endIndex, offsetBy: -2)
      arguments = String(arguments[..<endIndex])
    }
    
    content = content.replacingOccurrences(of: CrossMethodInfo.TmplKeyArguments, with: arguments)

    return content
  }

  
  private func makeNativeFunctionBody(cppClassName: String) -> String {
    var prefixContent = ""
    var suffixContent = ""
    for idx in 0..<paramsType.count {
      let type = self.paramsType[idx]
      let isPrimitiveType = type.isPrimitiveType()
      if isPrimitiveType == true {
        prefixContent += "\(CrossTmplUtils.TabSpace)\(type.toCppString()) var\(idx) = ocvar\(idx);\n"
      } else if type.type! == .CROSSBASE {
        prefixContent += "\(CrossTmplUtils.TabSpace)auto var\(idx) = crosspl::CrossPLUtils::SafeCastCrossObjectToHandle<\(cppClassName)>(ocvar\(idx));\n"
      } else {
        prefixContent += "\(CrossTmplUtils.TabSpace)auto var\(idx) = crosspl::CrossPLUtils::SafeCast\(type.toString())(ocvar\(idx));\n"
      }
   
      if(type.type! == .STRINGBUFFER
      || type.type! == .BYTEBUFFER) {
        suffixContent += "\(CrossTmplUtils.TabSpace)crosspl::CrossPLUtils::SafeCopy\(type.toString())ToSwift(ocvar\(idx), var\(idx).get());\n"
      }
    }
    prefixContent += "\n"
    suffixContent += "\n"
  
    var funcContent: String
    if self.isStatic == false {
      prefixContent += "\(CrossTmplUtils.TabSpace)auto obj = crosspl::CrossPLUtils::SafeCastCrossObjectToCpp<crosspl::native::\(cppClassName)>(nativeHandle);\n"
      funcContent = "obj->"
    } else {
      funcContent = "crosspl::native::\(cppClassName)::"
    }
  
    var argusContent = ""
    for idx in 0..<paramsType.count {
      let type = paramsType[idx]
      let isPrimitiveType = type.isPrimitiveType()
      if isPrimitiveType == true {
        argusContent += "var\(idx), "
      } else if type.type! == .CROSSBASE {
        argusContent += "var\(idx), "
      } else {
        argusContent += "var\(idx).get(), "
      }
    }
    if argusContent.isEmpty == false {
      let endIndex = argusContent.index(argusContent.endIndex, offsetBy: -2)
      argusContent = String(argusContent[..<endIndex])
    }
//    argusContent = argusContent.removeSuffix(", ")
    
    var retContent = ""
    if(returnType!.type! != .VOID) {
      let cppType = returnType!.toCppString(isConst: false)
      retContent = "\(cppType) ret = "
    
      let isPrimitiveType = returnType!.isPrimitiveType()
      if isPrimitiveType == true {
        suffixContent += "\(CrossTmplUtils.TabSpace)\(returnType!.toObjcString()) ocret = ret;\n"
        suffixContent += "\(CrossTmplUtils.TabSpace)return ocret;"
      } else if returnType!.type == .STRING {
        suffixContent += "\(CrossTmplUtils.TabSpace)auto ocret = crosspl::CrossPLUtils::SafeCast\(returnType!.toCppString())(ret);\n"
        suffixContent += "\(CrossTmplUtils.TabSpace)return ocret.get();"
      } else if returnType!.type == .CROSSBASE {
        suffixContent += "\(CrossTmplUtils.TabSpace)auto ocret = crosspl::CrossPLUtils::SafeCastCrossObject<\(cppClassName)>(ret);\n"
        suffixContent += "\(CrossTmplUtils.TabSpace)return ocret.get();"
      } else {
        suffixContent += "\(CrossTmplUtils.TabSpace)auto ocret = crosspl::CrossPLUtils::SafeCast\(returnType!)(&ret);\n"
        suffixContent += "\(CrossTmplUtils.TabSpace)return ocret.get();"
      }
    }
    var content = "\(prefixContent)"
    content += "\(CrossTmplUtils.TabSpace)\(retContent)\(funcContent)\(methodName!)(\(argusContent));\n\n"
    content += "\(suffixContent)"
    
    return content
  }
  
  private func makePlatformFunctionBody(cppClassName: String, javaClassPath: String) -> String {
    var prefixContent = ""
    var suffixContent = ""
  
    prefixContent += "\(CrossTmplUtils.TabSpace)auto ocobj = crosspl::CrossPLUtils::SafeCastCrossObjectToSwift<\(cppClassName)>(platformHandle);\n"
    for idx in 0..<paramsType.count {
      let type = paramsType[idx]
      let isPrimitiveType = type.isPrimitiveType()
      if isPrimitiveType == true {
        prefixContent += "\(CrossTmplUtils.TabSpace)\(type.toObjcString()) ocvar\(idx) = var\(idx);\n"
      } else if type.type! == .CROSSBASE {
        prefixContent += "\(CrossTmplUtils.TabSpace)auto ocvar\(idx) = crosspl::CrossPLUtils::SafeCastCrossObject<\(type.toCppString())>(var\(idx));\n"
      } else {
        prefixContent += "\(CrossTmplUtils.TabSpace)auto ocvar\(idx) = crosspl::CrossPLUtils::SafeCast\(type)(var\(idx));\n"
      }
  
      if(type.type! == .STRINGBUFFER
      || type.type! == .BYTEBUFFER) {
        suffixContent += "\(CrossTmplUtils.TabSpace)crosspl::CrossPLUtils::SafeCopy\(type.toString())ToCpp(const_cast<\(type.toCppString())*>(var\(idx), ocvar\(idx).get());\n"
      }
    }
    prefixContent += "\n"
    suffixContent += "\n"
  
//    prefixContent += "\(CrossTmplUtils.TabSpace)auto jclazz = crosspl::CrossPLUtils::FindJavaClass(jenv.get(), \"$javaClassPath\");\n"
//
//    var jniSigContent = "("
//    for idx in 0..<paramsType.count {
//      let type = paramsType[idx]
//      jniSigContent += "${type.toJniSigChar()}"
//    }
//    jniSigContent += ")${returnType.toJniSigChar()}"
//    prefixContent += "\(CrossTmplUtils.TabSpace\(auto jmethod$methodName = jenv->GetMethodID(jclazz, \"$methodName\", \"$jniSigContent\");\n"
//
    var funcContent = "ocobj "
    if self.isStatic == true {
      funcContent = "\(cppClassName) "
    }
    funcContent += "\(self.methodName!)"
//
//    if returnType!.isPrimitiveType() {
//      funcContent += returnType!.toString()
//    } else {
//      funcContent += "Object"
//    }
//
//    var objContent: String
//    if self.isStatic == true {
//      objContent = "jclazz"
//    } else {
//      objContent = "jobj"
//    }
//    funcContent += "Method"
  
    var argusContent = ""
    for idx in 0..<paramsType.count {
      let type = paramsType[idx]
      let isPrimitiveType = type.isPrimitiveType()
      if isPrimitiveType == true {
        argusContent += ": ocvar\(idx)"
      } else if type.type! == .CROSSBASE {
        argusContent += ": ocvar\(idx)"
      } else {
        argusContent += ": ocvar\(idx).get()"
      }
    }
  
    var retContent = ""
    if returnType!.type! != .VOID {
      let objcType = returnType!.toObjcString(isConst: false)
      retContent = "\(objcType) ocret = "
  
      let isPrimitiveType = returnType!.isPrimitiveType()
      if isPrimitiveType == true {
        suffixContent += "\(CrossTmplUtils.TabSpace)\(returnType!.toObjcString()) ret = ocret;\n"
      } else if returnType!.type! == .CROSSBASE {
        suffixContent += "\(CrossTmplUtils.TabSpace)auto ret = crosspl::CrossPLUtils::SafeCastCrossObject<\(returnType!.toCppString())>(ocret);\n"
      } else {
        suffixContent += "\(CrossTmplUtils.TabSpace)auto ret = crosspl::CrossPLUtils::SafeCast\(returnType!.toString())(jret);\n"
      }
      suffixContent += "\(CrossTmplUtils.TabSpace)return ret;"
    }
    var content = "\(prefixContent)"
    content += "\(CrossTmplUtils.TabSpace)\(retContent)[\(funcContent)\(argusContent)];\n\n"
    content += "\(suffixContent)"
  
    return content
  }


  private static let TmplKeyArguments = "%Arguments%"
  private static let TmplKeyFuncBody = "%FunctionBody%"
  
}
