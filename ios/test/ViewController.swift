//
//  ViewController.swift
//  test
//
//  Created by mengxk on 2019/9/20.
//  Copyright © 2019 Elastos. All rights reserved.
//

import UIKit
import ContactSDK

class ViewController: UIViewController {

  override func viewDidLoad() {
    super.viewDidLoad()
    // Do any additional setup after loading the view.
    
    let devId = getDeviceId()
    Log.i(tag: ViewController.TAG, msg: "Device ID:" + devId)
    
    mSavedMnemonic = UserDefaults.standard.string(forKey: ViewController.SavedMnemonicKey)
    if mSavedMnemonic == nil {
      var mnem = String()
      let ret = Contact.Debug.Keypair.GenerateMnemonic(language: ViewController.KeypairLanguage,
                                                       words: ViewController.KeypairWords,
                                                       mnem:&mnem)
      if(ret < 0) {
        showMessage(ViewController.ErrorPrefix + "Failed to call Contact.Debug.Keypair.GenerateMnemonic()")
        return
      }
      _ = newAndSaveMnemonic(mnem)
    }

    showMessage("Mnemonic:\(mSavedMnemonic ?? "nil")\n")
  }

  @IBAction func onOptionsMenuTapped(_ sender: Any) {
    optionsMenu.isHidden = !optionsMenu.isHidden
  }
  
  @IBAction func onOptionsItemSelected(_ sender: UIButton) {
    optionsMenu.isHidden = true

    enum ButtonTag: Int {
      case get_started = 100
      case clear_event = 101
      case new_mnemonic = 102
      case import_mnemonic = 103
      case new_and_start_contact = 104
      case stop_and_del_contact = 105
      case recreate_contact = 106
      case restart_contact = 107
      case get_user_info = 108
      case set_user_identifycode = 109
      case set_user_details = 110
      case set_wallet_address = 111
      case sync_upload = 112
      case sync_download = 113
      case friend_info = 114
      case add_friend = 115
      case del_friend = 116
      case send_message = 117
      case show_cached_didprop = 118
    }
    
    var message = ""
    switch sender.tag {
    case ButtonTag.get_started.rawValue:
      getStarted()
      break
    case ButtonTag.clear_event.rawValue:
      clearEvent()
      break
    case ButtonTag.new_mnemonic.rawValue:
      message = newAndSaveMnemonic(nil) ?? ""
      break
    case ButtonTag.import_mnemonic.rawValue:
      message = importMnemonic()
      break
    case ButtonTag.new_and_start_contact.rawValue:
      message = testNewContact()
      message += testStartContact()
      break
    case ButtonTag.stop_and_del_contact.rawValue:
      message = testStopContact()
      message += testDelContact()
      break
    case ButtonTag.recreate_contact.rawValue:
      message = testStopContact()
      message += testDelContact()
      message += testNewContact()
      message += testStartContact()
      break
    case ButtonTag.restart_contact.rawValue:
      message = testStopContact()
      message += testStartContact()
      break
    case ButtonTag.get_user_info.rawValue:
      message = showGetUserInfo()
      break
    case ButtonTag.set_user_identifycode.rawValue:
      message = showSetUserIdentifyCode()
      break
    case ButtonTag.set_user_details.rawValue:
      message = showSetUserDetails()
      break
    case ButtonTag.set_wallet_address.rawValue:
      message = showSetWalletAddress()
      break
    case ButtonTag.sync_upload.rawValue:
      message = testSyncUpload()
      break
    case ButtonTag.sync_download.rawValue:
      message = testSyncDownload()
      break
    case ButtonTag.friend_info.rawValue:
      message = listFriendInfo()
      break
    case ButtonTag.add_friend.rawValue:
      message = scanUserInfo()
      break
    case ButtonTag.del_friend.rawValue:
      message = removeFriend()
      break
    case ButtonTag.send_message.rawValue:
      message = sendMessage()
      break
    case ButtonTag.show_cached_didprop.rawValue:
      message = getCachedDidProp()
      break

    default:
      fatalError("Button [\(sender.currentTitle!)(\(sender.tag))] not decleared.")
    }
    
    showMessage(message)
  }

  private func getStarted() {
    var help = ""
    help += "Step1: [New Contact]\n"
    help += "Step2: [Start Contact]\n"
    help += "Step3: [User Info] can show you info\n"
    help += "Step4: After online, [Add friend] can add friend\n"
    help += "Step5: After friend online, [Send Message] can send message\n"
    
    clearEvent()
    showEvent(help)
  }
  
  private func clearEvent() {
    DispatchQueue.main.async { [weak self] in
      self?.eventLog.text = ""
    }
  }
  
  private func newAndSaveMnemonic(_ newMnemonic: String?) -> String? {
    mSavedMnemonic = newMnemonic
    if mSavedMnemonic == nil {
      var mnem = String()
      let ret = Contact.Debug.Keypair.GenerateMnemonic(language: ViewController.KeypairLanguage,
                                                       words: ViewController.KeypairWords,
                                                       mnem:&mnem)
      if(ret < 0) {
        showMessage(ViewController.ErrorPrefix + "Failed to call Contact.Debug.Keypair.GenerateMnemonic()")
        return nil
      }
      mSavedMnemonic = mnem
    }
  
    UserDefaults.standard.set(mSavedMnemonic, forKey: ViewController.SavedMnemonicKey)
    
    if mContact == nil { // noneed to restart
      return "Success to save mnemonic: \(String(describing: mSavedMnemonic))"
    }

    let dialog = UIAlertController(title: nil, message: nil, preferredStyle: .alert)
    dialog.message = "New mnemonic can only be valid after restart,\ndo you want restart app?"
    dialog.addAction(UIAlertAction(title: "OK", style: .default, handler: { _ in
      exit(0)
    }))
    dialog.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
    
    self.present(dialog, animated: false, completion: nil)

    return ("Cancel to save mnemonic: \(newMnemonic ?? "nil")\n")
  }

  private func importMnemonic() -> String {
    Helper.showImportMnemonic(view: self, listener: { result in
        if self.isEnglishWords(result) == false {
          self.showMessage(ViewController.ErrorPrefix + "Only english mnemonic is supported.")
          return
        }
        let privKey = self.getPrivateKey()
        if privKey.isEmpty {
          self.showMessage(ViewController.ErrorPrefix + "Bad mnemonic.")
          return
        }

        let message = self.newAndSaveMnemonic(result)
        self.showMessage(message!)
    })

    return "Success to show import mnemonic dialog."
  }

  
  private func testNewContact() -> String {
    Contact.Factory.SetLogLevel(level: 7)

    Contact.Factory.SetDeviceId(devId: getDeviceId())

    let cacheDir = FileManager.default.urls(for: .cachesDirectory, in: .userDomainMask).first
    let ret = Contact.Factory.SetLocalDataDir(dir: cacheDir!.path)
    if ret < 0 {
      return "Failed to call Contact.Factory.SetLocalDataDir() ret=\(ret)"
    }

    mContact = Contact.Factory.Create()
    if mContact == nil {
      return "Failed to call Contact.Factory.Create()"
    }

    if mContactListener != nil {
      mContactListener = nil
    }
    
    mContactListener = {
      class Impl: Contact.Listener {
        init(_ vc: ViewController) {
          viewCtrl = vc
          super.init()
        }
        
        override func onAcquire(request: AcquireArgs) -> Data? {
          let ret = viewCtrl.processAcquire(request: request)
          
          var msg = "onAcquire(): req=\(request.toString())\n"
          msg += "onAcquire(): resp=\(String(describing: ret))\n"
          viewCtrl.showEvent(msg)
          
          return ret
        }
        
        override func onEvent(event: EventArgs) {
          viewCtrl.processEvent(event: event)
          
          let msg = "onEvent(): ev=\(event.toString())\n"
          viewCtrl.showEvent(msg)
        }
        
        override func onReceivedMessage(humanCode: String, channelType: Int, message: Contact.Message) {
          var data: Any = message.data
          if message.type == Contact.Message.Kind.MsgText {
            data = String(data: message.data, encoding: .utf8)!
          }
          
          var msg = "onRcvdMsg(): data=\(data)\n"
          msg += "onRcvdMsg(): type=\(message.type)\n"
          msg += "onRcvdMsg(): crypto=" + message.cryptoAlgorithm + "\n"
          viewCtrl.showEvent(msg)
        }
        
        override func onError(errCode: Int32, errStr: String, ext: String?) {
          var msg = "\(errCode): \(errStr)"
          msg += "\n\(String(describing: ext))"
          viewCtrl.showError(msg)
        }
        
        private let viewCtrl: ViewController
      }
      
      return Impl(self)
    }()
    mContact!.setListener(listener: mContactListener)
  
    return "Success to create a contact instance."
  }

  private func testStartContact() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }
  
    let ret = mContact!.start()
    if ret < 0 {
      return "Failed to start contact instance. ret=\(ret)"
    }

    return "Success to start contact instance."
  }

  private func testStopContact() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let ret = mContact!.stop()
    if ret < 0 {
      return "Failed to stop contact instance. ret=\(ret)"
    }

    return "Success to stop contact instance."
  }

  private func testDelContact() -> String {
      if mContact == nil {
        return ViewController.ErrorPrefix + "Contact is null."
      }

      mContact = nil
      return "Success to delete a contact instance."
  }

  private func showGetUserInfo() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let info = mContact!.getUserInfo()
    if info == nil {
      return ViewController.ErrorPrefix + "Failed to get user info."
    }

    let humanCode: Helper.OrderedDictionary = [
        ("Did", info!.did),
        ("Ela", info!.elaAddress),
        ("Carrier", info!.getCurrDevCarrierAddr())
    ]
    let ext = info!.getCurrDevCarrierId()
    Helper.showAddress(view: self,
                       listener: { _ in
                         Helper.showDetails(view: self, msg: info!.toJson()!)
                       },
                       humanCode: humanCode, presentDevId: getDeviceId(), ext: ext)

    return info!.toString()
  }
  
  private func showSetUserIdentifyCode() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let checkList = [
        "\(Contact.UserInfo.Kind.PhoneNumber)",
        "\(Contact.UserInfo.Kind.EmailAddress)",
        "\(Contact.UserInfo.Kind.WechatId)"
    ]
    let separator = ":-:-:"

    Helper.showSetDetails(view: self,
                          checkList: checkList, separator: separator,
                          listener: { result in
        let details = [
          "\(Contact.UserInfo.Kind.PhoneNumber)": Contact.UserInfo.Kind.PhoneNumber,
          "\(Contact.UserInfo.Kind.EmailAddress)": Contact.UserInfo.Kind.EmailAddress,
          "\(Contact.UserInfo.Kind.WechatId)": Contact.UserInfo.Kind.WechatId
        ]

        let keyValue = result!.components(separatedBy: separator)
        let type = details[keyValue[0]]!
        let value = keyValue[1]

        let ret = self.mContact!.setIdentifyCode(type: type, value: value)
        if(ret < 0) {
          self.showMessage(ViewController.ErrorPrefix + "Failed to set \(result!). ret=\(ret)")
        }
    })

    let info = mContact!.getUserInfo()
    if info == nil {
      return ViewController.ErrorPrefix + "Failed to get user info."
    }

    return info.toString()
  }

  private func showSetUserDetails() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let checkList = [
      "\(Contact.UserInfo.Item.Nickname)",
      "\(Contact.UserInfo.Item.Avatar)",
      "\(Contact.UserInfo.Item.Gender)",
      "\(Contact.UserInfo.Item.Description)"
    ]
    let separator = ":-:-:"
    
    
    Helper.showSetDetails(view: self,
                          checkList: checkList, separator: separator,
                          listener: { result in
          let details = [
                  "\(Contact.UserInfo.Item.Nickname)": Contact.UserInfo.Item.Nickname,
                  "\(Contact.UserInfo.Item.Avatar)": Contact.UserInfo.Item.Avatar,
                  "\(Contact.UserInfo.Item.Gender)": Contact.UserInfo.Item.Gender,
                  "\(Contact.UserInfo.Item.Description)": Contact.UserInfo.Item.Description
          ]
                              
          let keyValue = result!.components(separatedBy: separator)
          let item = details[keyValue[0]]!
          let value = keyValue[1]

          let ret = self.mContact!.setUserInfo(item: item, value: value)
          if(ret < 0) {
            self.showMessage(ViewController.ErrorPrefix + "Failed to set \(result!). ret=\(ret)")
          }
      })

      let info = mContact!.getUserInfo()
      if info == nil {
        return ViewController.ErrorPrefix + "Failed to get user info."
      }

      return info.toString()
  }

  private func showSetWalletAddress() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let checkList = [
            "ELA",
            "BTC",
            "ETH"
    ]
    let separator = ":-:-:"

    Helper.showSetDetails(view: self,
                          checkList: checkList, separator: separator,
                          listener: { result in
    
          let keyValue = result!.components(separatedBy: separator)
          let name = keyValue[0]
          let value = keyValue[1]
                            
          let ret = self.mContact!.setWalletAddress(name: name, value: value)
          if(ret < 0) {
            self.showMessage(ViewController.ErrorPrefix + "Failed to set \(result!). ret=\(ret)")
          }
      })

      let info = mContact!.getUserInfo()
      if info == nil {
        return ViewController.ErrorPrefix + "Failed to get user info."
      }

      return info.toString()
  }

  private func testSyncUpload() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let ret = mContact!.syncInfoUploadToDidChain()
    if(ret < 0) {
      return ViewController.ErrorPrefix + "Failed to call syncInfoUploadToDidChain() ret=\(ret)"
    }

    return "Success to syncInfoUploadToDidChain."
  }

  private func testSyncDownload() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let ret = mContact!.syncInfoDownloadFromDidChain()
    if(ret < 0) {
      return ViewController.ErrorPrefix + "Failed to call syncInfoDownloadFromDidChain() ret=\(ret)"
    }

    return "Success to syncInfoDownloadFromDidChain ."
  }

  private func listFriendInfo() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    let friendList = mContact!.listFriendInfo()
    if(friendList == nil) {
      return ViewController.ErrorPrefix + "Failed to list friend info."
    }
    let friendCodeList = mContact!.listFriendCode()

    Helper.showFriendList(view: self, friendList: friendCodeList, listener:  { friendCode in
      var friendInfo: Contact.FriendInfo?
      for info in friendList! {
        if(info.humanCode == friendCode) {
            friendInfo = info
            break
        }
      }
      Helper.showDetails(view: self, msg: friendInfo!.toJson()!)
    })
    return "Success to list friend info."
  }

  private func scanUserInfo() -> String {
      if mContact == nil {
        return ViewController.ErrorPrefix + "Contact is null."
      }

      Helper.scanAddress(view: self, listener: { result in
        guard result != nil else {
          self.showMessage(ViewController.ErrorPrefix + "Failed to scan QRCode.")
          return
        }
        
        self.showMessage(result!)

        Helper.showAddFriend(view: self, friendCode: result!, listener: { summary in
          let ret = self.mContact!.addFriend(friendCode: result!, summary: summary!)
            if(ret < 0) {
              self.showMessage(ViewController.ErrorPrefix + "Failed to add friend. ret=\(ret)")
            }
          })
      })

      return ""
  }
  
  private func removeFriend() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }
    let info = mContact!.getUserInfo()
    if info == nil {
      return ViewController.ErrorPrefix + "Failed to get user info."
    }

    if (info!.status != ContactStatus.Online) {
      return ViewController.ErrorPrefix + "Contact is not online."
    }

    let separator = " "
    var friendCodeList = [String]()
    let friendList = mContact!.listFriendInfo()
    for friendInfo in friendList! {
        friendCodeList.append(friendInfo.humanCode! + separator + "[\(friendInfo.status)]")
    }
    
    Helper.showFriendList(view: self, friendList: friendCodeList, listener:  { result in
      let friendCode = result!.components(separatedBy: separator)[0]
      let dialog = UIAlertController(title: nil, message: nil, preferredStyle: .alert)
      dialog.message = "Are you sure to remove friend: " + friendCode
      dialog.addAction(UIAlertAction(title: "Accept", style: .default, handler: { _ in
        let ret = self.mContact!.removeFriend(friendCode: friendCode)
        if(ret < 0) {
          self.showMessage(ViewController.ErrorPrefix + "Failed to delete friend. ret=\(ret)")
          return;
        }
      }))
      dialog.addAction(UIAlertAction(title: "Cancel", style: .cancel))
      Helper.showDialog(self, dialog)
    })
    
    return "Success to send message.";
  }
  
  private func sendMessage() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }
    let info = mContact!.getUserInfo()
    if info == nil {
      return ViewController.ErrorPrefix + "Failed to get user info."
    }

    if (info!.status != ContactStatus.Online) {
      return ViewController.ErrorPrefix + "Contact is not online."
    }

    let friendCodeList = mContact!.listFriendCode()
    Helper.showFriendList(view: self, friendList: friendCodeList, listener:  { friendCode in
      Helper.showSendMessage(view: self, friendCode: friendCode!, listener:  { message in
        let msgInfo = self.mContact!.makeTextMessage(data: message!, cryptoAlgorithm: nil)

        let status = self.mContact!.getStatus(humanCode: friendCode!)
        if(status != ContactStatus.Online) {
          self.showMessage(ViewController.ErrorPrefix + "Friend isViewController. not online.")
          return
        }

        let ret = self.mContact!.sendMessage(friendCode: friendCode!,
                                             channelType: ContactChannel.Carrier,
                                             message: msgInfo)
        if(ret < 0) {
          self.showMessage(ViewController.ErrorPrefix + "Failed to send message to " + friendCode!)
        }
      })
    })
    
      return "Success to send message.";
  }

  private func getCachedDidProp() -> String {
    if mContact == nil {
      return ViewController.ErrorPrefix + "Contact is null."
    }

    var cachedDidProp = String()
    Contact.Debug.GetCachedDidProp(value: &cachedDidProp)
    
    Helper.showDetails(view: self, msg:  cachedDidProp)
    
    return "Success to get cached didprop.";
  }
  
  
  private func processAcquire(request: AcquireArgs) -> Data? {
    var response: Data?
  
    switch (request.type) {
      case .PublicKey:
        response = getPublicKey().data(using: .utf8)
        break
      case .EncryptData:
        response = request.data // plaintext
        break
      case .DecryptData:
        response = request.data // plaintext
        break
      case .DidPropAppId:
        let appId = "DC92DEC59082610D1D4698F42965381EBBC4EF7DBDA08E4B3894D530608A64AAA65BB82A170FBE16F04B2AF7B25D88350F86F58A7C1F55CC29993B4C4C29E405"
        response = appId.data(using: .utf8)
        break
      case .DidAgentAuthHeader:
        response = getAgentAuthHeader()
        break
      case .SignData:
        response = signData(data: request.data)
        break
    }
  
    return response
  }

  private func processEvent(event: EventArgs) {
    switch (event.type) {
      case .StatusChanged:
        break
      case .FriendRequest:
        let requestEvent = event as! Contact.Listener.RequestEvent
        Helper.showFriendRequest(view: self,
                                 humanCode: requestEvent.humanCode, summary: requestEvent.summary,
                                 listener: { _ in
          let ret = self.mContact!.acceptFriend(friendCode: requestEvent.humanCode)
          if(ret < 0) {
            self.showMessage(ViewController.ErrorPrefix + "Failed to acceptFriend \(requestEvent.humanCode). ret=\(ret)")
          }
        })
        break
    case .HumanInfoChanged:
      let infoEvent = event as! Contact.Listener.InfoEvent
      let msg = event.humanCode + " info changed: " + infoEvent.toString()
      showEvent(msg)
      break
    }
  }
  
  private func getPublicKey() -> String {
    var seed = Data()
    var ret = Contact.Debug.Keypair.GetSeedFromMnemonic(mnemonic: mSavedMnemonic!,
                                                        mnemonicPassword: "",
                                                        seed: &seed)
    if(ret < 0) {
      showMessage(ViewController.ErrorPrefix + "Failed to call Contact.Debug.Keypair.GetSeedFromMnemonic()")
    }
    var pubKey = String()
    ret = Contact.Debug.Keypair.GetSinglePublicKey(seed: seed, pubKey: &pubKey)
    if(ret < 0) {
      showMessage(ViewController.ErrorPrefix + "Failed to call Contact.Debug.Keypair.GetSinglePublicKey()")
    }

    return pubKey
  }

  private func getPrivateKey() -> String {
    var seed = Data()
    var ret = Contact.Debug.Keypair.GetSeedFromMnemonic(mnemonic: mSavedMnemonic!,
                                                        mnemonicPassword: "",
                                                        seed: &seed)
    if(ret < 0) {
      showMessage(ViewController.ErrorPrefix + "Failed to call Contact.Debug.Keypair.GetSeedFromMnemonic()")
    }
    var privKey = String()
    ret = Contact.Debug.Keypair.GetSinglePrivateKey(seed: seed, privKey: &privKey)
    if(ret < 0) {
      showMessage(ViewController.ErrorPrefix + "Failed to call Contact.Debug.Keypair.GetSinglePrivateKey()")
    }

    return privKey
  }
  
  private func getAgentAuthHeader() -> Data {
    let appid = "org.elastos.debug.didplugin"
    //let appkey = "b2gvzUM79yLhCbbGNWCuhSsGdqYhA7sS"
    let timestamp = Int64(Date().timeIntervalSince1970 * 1000)
    let auth = Utils.getMd5(str: "appkey\(timestamp)")
    let headerValue = "id=\(appid)time=\(timestamp)auth=\(auth)"
    Log.i(tag: ViewController.TAG, msg: "getAgentAuthHeader() headerValue=" + headerValue)
  
    return headerValue.data(using: .utf8)!
  }
  
  private func signData(data: Data?) -> Data? {
    if data == nil {
      return nil
    }
    
    let privKey = getPrivateKey()

    var signedData = Data()
    let ret = Contact.Debug.Keypair.Sign(privateKey: privKey, data: data!, signedData: &signedData)
    if(ret < 0) {
      showMessage(ViewController.ErrorPrefix + "Failed to call Contact.Debug.Keypair.Sign()")
      return nil
    }
  
    return signedData
  }
  
  private func getDeviceId() -> String {
    let devId = UIDevice.current.identifierForVendor?.uuidString
    return devId!
  }

  
  private func showMessage(_ msg: String) {
    Log.i(tag: ViewController.TAG, msg: "\(msg)")
    
    DispatchQueue.main.async { [weak self] in
      self?.msgLog.text = msg
    }
    
    if msg.hasPrefix(ViewController.ErrorPrefix) {
      showToast(msg)
    }
  }
  
  private func showEvent(_ newMsg: String) {
    print("\(newMsg)")
    DispatchQueue.main.async { [weak self] in
      self?.eventLog.text += "\n"
      self?.eventLog.text += newMsg
    }
  }
  
  private func showError(_ newErr: String) {
    Log.e(tag: ViewController.TAG, msg: newErr)

    DispatchQueue.main.async { [weak self] in
      self?.errLog.text = newErr
    }
  }

  private func showToast(_ message : String) {
    let alert = UIAlertController(title: nil, message: message, preferredStyle: .alert)
    alert.view.backgroundColor = UIColor.black
    alert.view.alpha = 0.6
    alert.view.layer.cornerRadius = 15
    
    DispatchQueue.main.async { [weak self] in
      self?.present(alert, animated: false)
    }
    
    DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 1) {
      alert.dismiss(animated: true)
    }
  }
  
  private func isEnglishWords(_ words: String?) -> Bool {
    guard (words?.count ?? -1) > 0 else {
      return false
    }
    
    let isEnglish = (words!.range(of: "[^a-zA-Z ]", options: .regularExpression) == nil)
    return isEnglish
  }
  
  @IBOutlet weak var optionsMenu: UIScrollView!
  @IBOutlet weak var errLog: UITextView!
  @IBOutlet weak var msgLog: UITextView!
  @IBOutlet weak var eventLog: UITextView!
  
  private var mSavedMnemonic: String?
  private var mContact: Contact?
  private var mContactListener: Contact.Listener?
  
  private static let KeypairLanguage = "english"
  private static let KeypairWords = ""
  private static let SavedMnemonicKey = "mnemonic"
  private static let ErrorPrefix = "Error: "
  private static let TAG = "ContactTest"

}

