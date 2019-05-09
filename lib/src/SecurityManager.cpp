//
//  SecurityManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <SecurityManager.hpp>

#include <Elastos.Wallet.Utility.h>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
SecurityManager::SecurityManager()
    : mSecurityListener()
{
}

SecurityManager::~SecurityManager()
{
}

void SecurityManager::setSecurityListener(std::shared_ptr<SecurityListener> listener)
{
    mSecurityListener = listener;
}

int SecurityManager::generateMnemonic(std::string& value) const
{
    auto mnemonic = ::generateMnemonic(Language, Words);
    if(mnemonic == nullptr) {
        return ErrCode::FailedKeypairGenMnemonic;
    }

    value = mnemonic;
    freeBuf(mnemonic);

    return 0;
}

int SecurityManager::getPublicKey(std::string& value)
{
    std::vector<uint8_t> seed;
    int ret = getSeed(seed);
    if(ret < 0) {
        return ret;
    }

    auto pubKey = ::getSinglePublicKey(seed.data(), seed.size());
    if(pubKey == nullptr) {
        return ErrCode::FailedKeypairGetPubKey;
    }

    value = pubKey;
    freeBuf(pubKey);

    return 0;
}

int SecurityManager::getElaAddress(std::string& value)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int SecurityManager::getDid(std::string& value)
{
    std::string pubKey;
    int ret = getPublicKey(pubKey);
    if(ret < 0) {
        return ret;
    }

    auto did = ::getDid(pubKey.c_str());
    if(did == nullptr) {
        return ErrCode::FailedKeypairGetDid;
    }

    value = did;
    freeBuf(did);

    return 0;
}

int SecurityManager::encryptData(const std::string& key, const std::vector<int8_t>& src, std::vector<int8_t>& dest)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int SecurityManager::encryptData(const std::vector<int8_t>& src, std::vector<int8_t>& dest)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int SecurityManager::decryptData(const std::vector<int8_t>& src, std::vector<int8_t>& dest)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

void SecurityManager::clear()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
int SecurityManager::getSeed(std::vector<uint8_t>& value)
{
    if(mSecurityListener == nullptr) {
        return ErrCode::NoSecurityListener;
    }

    auto mnemonic = mSecurityListener->onRequestMnemonic();
    if(mnemonic.empty() == true) {
        return ErrCode::BadMnemonic;
    }

    void* seed = nullptr;
    int size = ::getSeedFromMnemonic(&seed, mnemonic.c_str(), Language, Words, "");
    if(size <= 0) {
        return ErrCode::FailedKeypairMakeSeed;
    }

    auto ptr = reinterpret_cast<uint8_t*>(seed);
    value = std::vector<uint8_t>(ptr, ptr + size);
    freeBuf(seed);

    return 0;
}

} // namespace elastos
