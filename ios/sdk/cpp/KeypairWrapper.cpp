//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <KeypairWrapper.hpp>

#include "Log.hpp"
#include <Elastos.SDK.Keypair.C/Elastos.Wallet.Utility.h>

namespace crosspl {
namespace native {
    
/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/

int KeypairWrapper::GetSinglePublicKey(const std::span<uint8_t>* seed, std::stringstream* pubKey)
{
  pubKey->str("");

  auto ret = ::getSinglePublicKey(seed->data(), (int)seed->size());
  if(ret == nullptr) {
    return -1;
  }
  
  (*pubKey) << ret;
  freeBuf(ret);

  return 0;
}

int KeypairWrapper::GetSinglePrivateKey(const std::span<uint8_t>* seed, std::stringstream* privKey)
{
  privKey->str("");

  auto ret = ::getSinglePrivateKey(seed->data(), (int)seed->size());
  if(ret == nullptr) {
    return -1;
  }
  
  (*privKey) << ret;
  freeBuf(ret);

  return 0;
}

int KeypairWrapper::GenerateMnemonic(const char* language, const char* words, std::stringstream* mnem)
{
  mnem->str("");

  auto ret = ::generateMnemonic(language, words);
  if(ret == nullptr) {
    return -1;
  }
  
  (*mnem) << ret;
  freeBuf(ret);

  return 0;
}

int KeypairWrapper::GetSeedFromMnemonic(const char* mnemonic, const char* mnemonicPassword, std::vector<uint8_t>* seed)
{
  seed->clear();

  void* seedData = nullptr;
  int seedSize = ::getSeedFromMnemonic(&seedData, mnemonic, mnemonicPassword);
  if(seedData == nullptr || seedSize <= 0) {
    return -1;
  }

  *seed = std::vector<uint8_t>((uint8_t*)seedData, (uint8_t*)seedData + seedSize);

  freeBuf(seedData);

  return 0;
}

int KeypairWrapper::Sign(const char* privateKey, const std::span<uint8_t>* data, std::vector<uint8_t>* signedData)
{
  signedData->clear();

  void* keypairSignedData = nullptr;
  int keypairSignedSize = ::sign(privateKey, data->data(), data->size(), &keypairSignedData);
  if(keypairSignedData == nullptr || keypairSignedSize <= 0) {
    return -1;
  }

  *signedData = std::vector<uint8_t>((uint8_t*)keypairSignedData, (uint8_t*)keypairSignedData + keypairSignedSize);

  freeBuf(keypairSignedData);

  return 0;
}


/***********************************************/
/***** class public function implement  ********/
/***********************************************/

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/


} //namespace native
} //namespace crosspl
