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

  @IBAction func onButtonItemSelected(_ sender: UIButton) {
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
    
    switch sender.tag {
    case ButtonTag.get_started.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.clear_event.rawValue:
      print("\(sender.tag)")
      break
    case ButtonTag.new_mnemonic.rawValue:
      print("\(sender.tag)")
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
  
}

