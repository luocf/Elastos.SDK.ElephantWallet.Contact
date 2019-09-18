//
//  test.hpp
//  lib
//
//  Created by mengxk on 2019/9/5.
//  Copyright © 2019 Elastos. All rights reserved.
//

#ifndef ExternalTest_hpp
#define ExternalTest_hpp

#include <string>
#include <vector>
#include <sstream>
#include "experimental-span.hpp"
#include "CrossBase.hpp"

namespace crosspl {
namespace native {
    
class ExternalTest {

public:
  static void externalTestFuncNoArgs();
  void externalTestFunc(bool a,
                        int32_t b,
                        int64_t c,
                        double d,
                        const char* e,
                        const std::span<int8_t>* f,
                        
                        std::stringstream* h,
                        std::vector<int8_t>* i,
                        
                        int64_t platformHandle);
  
}; // class ExternalTest
  
} //namespace native
} //namespace crosspl

#endif /* ExternalTest_hpp */
