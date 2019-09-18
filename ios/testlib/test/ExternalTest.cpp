//
//  test.cpp
//  lib
//
//  Created by mengxk on 2019/9/5.
//  Copyright © 2019 Elastos. All rights reserved.
//

#include "ExternalTest.hpp"

#include "InternalTest.proxy.h"

namespace crosspl {
namespace native {
    
void ExternalTest::externalTestFuncNoArgs()
{
  printf(__PRETTY_FUNCTION__);
}

void ExternalTest::externalTestFunc(bool a,
                      int32_t b,
                      int64_t c,
                      double d,
                      const char* e,
                      const std::span<int8_t>* f,
                      
                      std::stringstream* h,
                      std::vector<int8_t>* i,
                                    
                      int64_t internalTestHandle)
{
    printf("%s a=%d,b=%d,c=%lld,d=%f,e=%s,f=%s(%lu),",
           __PRETTY_FUNCTION__, a, b, c, d, e, f->data(), f->size());
    
    if(h != nullptr) {
      printf("h=%s,", h->str().c_str());
      h->str("");
      (*h) << "new string from ExternalTest::externalTestFunc()";
    } else {
      printf("h=nullptr,");
    }
    
    if(i != nullptr) {
      printf("i=%s(%lu)\n", i->data(), i->size());
      i->clear();
      auto ret = h->str();
      auto tmp = std::vector<int8_t>(ret.begin(), ret.end());
      printf("tmp=%s(%lu)\n", tmp.data(), tmp.size());
      i->swap(tmp);
      printf("i=%s(%lu)\n", i->data(), i->size());
    } else {
      printf("i=nullptr\n");
    }
  
  if(internalTestHandle != 0) {
    crosspl_Proxy_InternalTest_internalTestFunc(internalTestHandle, 111);

  }
}


} //namespace native
} //namespace crosspl
