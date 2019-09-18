import Foundation

class CrossVariableType {
  public enum `Type` {
    case BOOLEAN
    case INT32
    case INT64
    case DOUBLE
    case VOID
    
    case STRING
    case BYTEARRAY
    case FUNCTION
    
    case STRINGBUFFER
    case BYTEBUFFER
    
    case CROSSBASE
  }
  
  static func Parse(sourceContent: String) -> CrossVariableType {
    let supportedTypeKind = ["Bool": Type.BOOLEAN,
                             "Int32": Type.INT32,
                             "Int64": Type.INT64,
                             "Double": Type.DOUBLE,
                             "Void": Type.VOID]
    
    let supportedTypeDeclared = ["String": Type.STRING,
                                 "Data": Type.BYTEARRAY,
                                 "Function": Type.FUNCTION,
                                 
                                 "inout String": Type.STRINGBUFFER,
                                 "inout NSString": Type.STRINGBUFFER,
                                 "inout Data": Type.BYTEBUFFER,
                                 "inout NSData": Type.BYTEBUFFER,
                                 
                                 "CrossBase": Type.CROSSBASE]
    
    var trimContent = sourceContent.trimmingCharacters(in: .whitespacesAndNewlines)
   
    let varType = CrossVariableType()
    varType.type = supportedTypeKind[trimContent]
    if varType.type != nil {
      return varType
    }
    
    trimContent = trimContent.trimmingCharacters(in: CharacterSet(charactersIn: "?"))
    varType.type = supportedTypeDeclared[trimContent]
    if varType.type == nil {
      print("CrossVariableType.Parse() Unsupported variable type: \(trimContent)")
      exit(1)
    }
    
//    print("=================CrossVariableType.Parse() trimContent=\(varType.toString())")

    return varType
  }
  
  func toString() -> String {
    let typeMap = [
      Type.BOOLEAN: "Bool",
      Type.INT32: "Int32",
      Type.INT64: "Int64",
      Type.DOUBLE: "Double",
      Type.VOID: "Void",
      
      Type.STRING: "String",
      Type.BYTEARRAY: "ByteArray",
      Type.FUNCTION: "Function",
      Type.STRINGBUFFER: "StringBuffer",
      Type.BYTEBUFFER: "ByteBuffer",
      Type.CROSSBASE: "CrossBase"
    ]
    
    //  var cppType = toString(primitiveTypeMap, classTypeMap, isConst)
    let strType = typeMap[type!]
    
    return strType!
  }
  
  func isPrimitiveType() -> Bool {
    let primitiveTypeSet = [
      Type.BOOLEAN,
      Type.INT32,
      Type.INT64,
      Type.DOUBLE,
      Type.VOID
    ]
  
    return primitiveTypeSet.contains(type!)
  }

  
  func toCppString(isConst: Bool = false) -> String {
    let typeMap = [
      Type.BOOLEAN: "bool",
      Type.INT32: "int32_t",
      Type.INT64: "int64_t",
      Type.DOUBLE: "double",
      Type.VOID: "void",

      Type.STRING: "const char*",
      Type.BYTEARRAY: "std::span<int8_t>",
      Type.FUNCTION: "std::function<void()>",
      Type.STRINGBUFFER: "std::stringstream",
      Type.BYTEBUFFER: "std::vector<int8_t>",
      Type.CROSSBASE: "::CrossBase"
    ]
  
//  var cppType = toString(primitiveTypeMap, classTypeMap, isConst)
    let cppType = typeMap[type!]
    
    return cppType!
  }
  
  func toObjcString(isConst: Bool = false) -> String {
    let typeMap = [
      Type.BOOLEAN: "bool",
      Type.INT32: "int32_t",
      Type.INT64: "int64_t",
      Type.DOUBLE: "double",
      Type.VOID: "void",

      Type.STRING: "NSString*",
      Type.BYTEARRAY: "NSData*",
      Type.FUNCTION: "---",
      Type.STRINGBUFFER: "NSString**",
      Type.BYTEBUFFER: "NSData**",
      Type.CROSSBASE: "NSObject*"
    ]
    
//    var jniType = toString(primitiveTypeMap, classTypeMap, isConst)
    let objcType = typeMap[type!]
    
    return objcType!
  }

  func toSwiftString() -> String {
    let typeMap = [
      Type.BOOLEAN: "Bool",
      Type.INT32: "Int32",
      Type.INT64: "Int64",
      Type.DOUBLE: "Double",
      Type.VOID: "Void",
      
      Type.STRING: "String",
      Type.BYTEARRAY: "Data",
      Type.FUNCTION: "---",
      Type.STRINGBUFFER: "inout String",
      Type.BYTEBUFFER: "inout Data",
      Type.CROSSBASE: "CrossBase"
    ]
  
    let swiftType = typeMap[type!]
    
    return swiftType!
  }

  
  var type: Type?
}
