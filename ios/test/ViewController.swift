//
//  ViewController.swift
//  test
//
//  Created by mengxk on 2019/9/20.
//  Copyright © 2019 Elastos. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

  override func viewDidLoad() {
    super.viewDidLoad()
    // Do any additional setup after loading the view.
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
      message = newAndSaveMnemonic(nil);
      break
    case ButtonTag.import_mnemonic.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.new_and_start_contact.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.stop_and_del_contact.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.recreate_contact.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.restart_contact.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.get_user_info.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.set_user_identifycode.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.set_user_details.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.set_wallet_address.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.sync_upload.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.sync_download.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.friend_info.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.add_friend.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.del_friend.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.send_message.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.show_cached_didprop.rawValue:
      print("\(sender.tag)")
      break

    default:
      fatalError("Button [\(sender.currentTitle!)(\(sender.tag))] not decleared.")
    }
  }

  private func getStarted() {
    var help = ""
    help += "Step1: [New Contact]\n"
    help += "Step2: [Start Contact]\n"
    help += "Step3: [User Info] can show you info\n"
    help += "Step4: After online, [Add friend] can add friend\n"
    help += "Step5: After friend online, [Send Message] can send message\n"
    
    clearEvent();
    showEvent(help);
  }
  
  private func clearEvent() {
    eventLog.text = ""
  }
  
  private func newAndSaveMnemonic(_ newMnemonic: String?) -> String {
    mSavedMnemonic = newMnemonic;
    if mSavedMnemonic == nil {
      //mSavedMnemonic = ElastosKeypair.generateMnemonic(KeypairLanguage, KeypairWords);
    }
  
    UserDefaults.standard.set(mSavedMnemonic, forKey: ViewController.SavedMnemonicKey)
    
//  if(mContact == null) { // noneed to restart
//    return ("Success to save mnemonic:\n" + mSavedMnemonic);
//  }

    let dialog = UIAlertController(title: nil, message: nil, preferredStyle: .alert)
    dialog.message = "New mnemonic can only be valid after restart,\ndo you want restart app?"
    dialog.addAction(UIAlertAction(title: "OK", style: .default, handler: { _ in
      exit(0);
    }))
    dialog.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
    
    self.present(dialog, animated: false, completion: nil)

    return ("Cancel to save mnemonic: \(newMnemonic ?? "nil")\n");
  }

  
  
  private func showEvent(_ newMsg: String) {
    print("\(newMsg)")
    eventLog.text += "\n"
    eventLog.text += newMsg
  }
  
  @IBOutlet weak var optionsMenu: UIStackView!
  @IBOutlet weak var eventLog: UITextView!
  
  private var mSavedMnemonic: String?
  private static let SavedMnemonicKey = "mnemonic";
}

