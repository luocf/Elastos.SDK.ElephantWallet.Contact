import Foundation

/* @CrossClass */
@objc open class CrossBase : NSObject {
  public init(className: String? = nil, nativeHandle: Int64 = 0) {
    if className != nil {
      self.className = className!
    } else {
      self.className = String(describing: type(of: self))
    }
    self.nativeHandle = nativeHandle

    if(self.nativeHandle == 0) {
      self.nativeHandle = CrossBase.CreateNativeObject(swiftClassName: self.className)
    }
  }
  
  deinit {
    CrossBase.DestroyNativeObject(swiftClassName: self.className, nativeHandle: self.nativeHandle)
    self.nativeHandle = 0
  }
  
  public func bind() {
    bindPlatformHandle(thisObj: self)
  }
  
  public func unbind() {
    unbindPlatformHandle(thisObj: self)
  }

  public private(set) var nativeHandle: Int64
  private let className: String
  
  /* @CrossNativeInterface */
  private static func CreateNativeObject(swiftClassName: String) -> Int64{
    return crosspl_Proxy_CrossBase_CreateNativeObject(swiftClassName)
  }
  
  /* @CrossNativeInterface */
  private static func DestroyNativeObject(swiftClassName: String, nativeHandle: Int64) {
    crosspl_Proxy_CrossBase_DestroyNativeObject(swiftClassName, nativeHandle)
  }
  
  /* @CrossNativeInterface */
  private func bindPlatformHandle(thisObj: CrossBase) {
    crosspl_Proxy_CrossBase_bindPlatformHandle(nativeHandle, thisObj)
  }
  
  /* @CrossNativeInterface */
  private func unbindPlatformHandle(thisObj: CrossBase) {
    crosspl_Proxy_CrossBase_unbindPlatformHandle(nativeHandle, thisObj)
  }
}
