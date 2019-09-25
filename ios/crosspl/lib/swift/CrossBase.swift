import Foundation

/* @CrossClass */
@objc open class CrossBase : NSObject {
  public init(className: String? = nil, nativeHandle: Int64 = 0) {
    if className != nil {
      self.classTitle = className!
    } else {
      self.classTitle = String(describing: type(of: self))
    }
    self.nativeHandle = nativeHandle

    if(self.nativeHandle == 0) {
      self.nativeHandle = CrossBase.CreateNativeObject(swiftClassName: self.classTitle)
    }
  }
  
  deinit {
    CrossBase.DestroyNativeObject(swiftClassName: self.classTitle, nativeHandle: self.nativeHandle)
    self.nativeHandle = 0
  }
  
  public func bind() {
    bindPlatformHandle(thisObj: self)
  }
  
  public func unbind() {
    unbindPlatformHandle(thisObj: self)
  }

  @objc public private(set) var nativeHandle: Int64
  private let classTitle: String
  
  /* @CrossNativeInterface */
  @objc private static func CreateNativeObject(swiftClassName: String) -> Int64 {
    return crosspl_Proxy_CrossBase_CreateNativeObject(swiftClassName)
  }
  
  /* @CrossNativeInterface */
  @objc private static func DestroyNativeObject(swiftClassName: String, nativeHandle: Int64) {
    crosspl_Proxy_CrossBase_DestroyNativeObject(swiftClassName, nativeHandle)
  }
  
  /* @CrossNativeInterface */
  @objc private func bindPlatformHandle(thisObj: CrossBase) {
    crosspl_Proxy_CrossBase_bindPlatformHandle(nativeHandle, thisObj)
  }
  
  /* @CrossNativeInterface */
  @objc private func unbindPlatformHandle(thisObj: CrossBase) {
    crosspl_Proxy_CrossBase_unbindPlatformHandle(nativeHandle, thisObj)
  }
}
