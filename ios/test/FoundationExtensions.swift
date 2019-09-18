//
//  FoundationExtensions.swift
//  test
//
//  Created by mengxk on 2019/9/17.
//  Copyright © 2019 Elastos. All rights reserved.
//

import Foundation

extension String {
  static func ToData(from: String?) -> Data? {
    return from?.data(using: String.Encoding.utf8);
  }
}

extension Data {
  static func ToString(from: Data?) -> String? {
    guard from != nil else {
      return nil
    }

    return String(data: from!, encoding: String.Encoding.utf8)
  }
}
