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

int SecurityManager::getPublicKey(std::string& value)
{
    if(mSecurityListener == nullptr) {
        return ErrCode::NoSecurityListener;
    }

    value = mSecurityListener->onRequestPublicKey();
    if(value.empty() == true) {
        return ErrCode::BadSecurityValue;
    }

    return 0;
}

int SecurityManager::getElaAddress(std::string& value)
{
    std::string pubKey;
    int ret = getPublicKey(pubKey);
    if(ret < 0) {
        return ret;
    }

    auto addr = ::getAddress(pubKey.c_str());
    if(addr == nullptr) {
        return ErrCode::KeypairError;
    }

    value = addr;
    freeBuf(addr);

    return 0;
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
        return ErrCode::KeypairError;
    }

    value = did;
    freeBuf(did);

    return 0;
}


int SecurityManager::encryptData(const std::string& key, const std::vector<int8_t>& src, std::vector<int8_t>& dest)
{
    if(mSecurityListener == nullptr) {
        return ErrCode::NoSecurityListener;
    }

    dest = mSecurityListener->onEncryptData(key, src);
    if(dest.empty() == true) {
        return ErrCode::BadSecurityValue;
    }

    return 0;
}

int SecurityManager::decryptData(const std::vector<int8_t>& src, std::vector<int8_t>& dest)
{
    if(mSecurityListener == nullptr) {
        return ErrCode::NoSecurityListener;
    }

    dest = mSecurityListener->onDecryptData(src);
    if(dest.empty() == true) {
        return ErrCode::BadSecurityValue;
    }

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
