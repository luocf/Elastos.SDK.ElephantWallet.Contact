//
//  FriendManager.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <FriendManager.hpp>

#include <algorithm>
#include <CompatibleFileSystem.hpp>
#include <Elastos.Wallet.Utility.h>
#include <Log.hpp>
#include <Json.hpp>
#include <MessageManager.hpp>
#include <SafePtr.hpp>

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
FriendManager::FriendManager(std::weak_ptr<SecurityManager> sectyMgr)
    : mSecurityManager(sectyMgr)
    , mConfig()
    , mMessageManager()
    , mFriendListener()
    , mFriendList()
{
}

FriendManager::~FriendManager()
{
}

bool FriendManager::contains(const std::string& friendCode)
{
    for(const auto& it: mFriendList) {
        bool find = it->contains(friendCode);
        if(find == true) {
            return true;
        }
    }

    return false;
}

bool FriendManager::contains(const std::shared_ptr<HumanInfo>& friendInfo)
{
    for(const auto& it: mFriendList) {
        bool find = it->contains(friendInfo);
        if(find == true) {
            return true;
        }
    }

    return false;
}


void FriendManager::setFriendListener(std::shared_ptr<FriendListener> listener)
{
    mFriendListener = listener;
}

void FriendManager::setConfig(std::weak_ptr<Config> config, std::weak_ptr<MessageManager> msgMgr)
{
    mConfig = config;
    mMessageManager = msgMgr;
}

int FriendManager::loadLocalData()
{
    auto config = SAFE_GET_PTR(mConfig);
    auto dataFilePath = std::filesystem::path(config->mUserDataDir.c_str()) / DataFileName;

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::vector<uint8_t> originData;
    int ret = sectyMgr->loadCryptoFile(dataFilePath.string(), originData);
    if(ret < 0) {
        return ret;
    }

    std::vector<std::shared_ptr<FriendInfo>> friendList;
    std::string friendData {originData.begin(), originData.end()};
    try {
        Json jsonFriend = Json::parse(friendData);
        for(const auto& it: jsonFriend) {
            auto info = std::make_shared<FriendInfo>(weak_from_this());
            ret = info->deserialize(it);
            if(ret < 0) {
                return ret;
            }
            friendList.push_back(info);
        }
    } catch(const std::exception& ex) {
        Log::E(Log::TAG, "Failed to load local data from: %s.\nex=%s", dataFilePath.c_str(), ex.what());
        return ErrCode::JsonParseException;
    }

    mFriendList.swap(friendList);

    return 0;
}

int FriendManager::saveLocalData()
{
    std::string friendData;
    int ret = serialize(friendData);
    if(ret < 0) {
        return ret;
    }

    auto config = SAFE_GET_PTR(mConfig);
    auto dataFilePath = std::filesystem::path(config->mUserDataDir.c_str()) / DataFileName;

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::vector<uint8_t> originData {friendData.begin(), friendData.end()};
    ret = sectyMgr->saveCryptoFile(dataFilePath.string(), originData);
    if(ret < 0) {
        return ret;
    }

    Log::D(Log::TAG, "Save local data to: %s, data: %s", dataFilePath.c_str(), friendData.c_str());

    return 0;
}

int FriendManager::serialize(std::string& value) const
{
    Json jsonFriend = Json::array();
    for(const auto& it: mFriendList) {
        std::string data;
        int ret = it->serialize(data);
        if(ret < 0) {
            return ret;
        }
        jsonFriend.push_back(data);
    }

    value = jsonFriend.dump();

    return 0;
}

int FriendManager::restoreFriends()
{
    int ret = loadLocalData();
    if(ret < 0
    && ret != ErrCode::FileNotExistsError) {
        return ret;
    }

    return 0;
}

int FriendManager::tryAddFriend(const std::string& friendCode, const std::string& summary,
                                bool remoteRequest)
{
    auto kind = HumanInfo::AnalyzeHumanKind(friendCode);
    if(static_cast<int>(kind) < 0) {
        return static_cast<int>(kind);
    }

    int ret = addFriend(kind, friendCode, summary, remoteRequest);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int FriendManager::tryRemoveFriend(const std::string& friendCode)
{
    auto kind = HumanInfo::AnalyzeHumanKind(friendCode);
    if(static_cast<int>(kind) < 0) {
        return static_cast<int>(kind);
    }

    int ret = removeFriend(kind, friendCode);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int FriendManager::tryGetFriendInfo(const std::string& friendCode, std::shared_ptr<FriendInfo>& friendInfo)
{
    auto kind = HumanInfo::AnalyzeHumanKind(friendCode);
    if(static_cast<int>(kind) < 0) {
        return static_cast<int>(kind);
    }

    int ret = getFriendInfo(kind, friendCode, friendInfo);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int FriendManager::addFriend(FriendInfo::HumanKind friendKind, const std::string& friendCode,
                             const std::string& summary, bool remoteRequest)
{
    int ret = ErrCode::InvalidArgument;

    if(friendKind == FriendInfo::HumanKind::Did) {
        ret = addFriendByDid(friendCode, summary, remoteRequest);
    } else if(friendKind == FriendInfo::HumanKind::Ela) {
        ret = addFriendByEla(friendCode, summary, remoteRequest);
    } else if(friendKind == FriendInfo::HumanKind::Carrier) {
        ret = addFriendByCarrier(friendCode, summary, remoteRequest);
    }

    return ret;
}

int FriendManager::removeFriend(FriendInfo::HumanKind friendKind, const std::string& friendCode)
{
    int ret = ErrCode::InvalidArgument;

    if(friendKind == FriendInfo::HumanKind::Did) {
        ret = removeFriendByDid(friendCode);
    } else if(friendKind == FriendInfo::HumanKind::Ela) {
        ret = removeFriendByEla(friendCode);
    } else if(friendKind == FriendInfo::HumanKind::Carrier) {
        ret = removeFriendByCarrier(friendCode);
    }

    return ret;
}

int FriendManager::addFriendInfo(std::shared_ptr<FriendInfo> friendInfo)
{
    mFriendList.push_back(friendInfo);
    return 0;
}

int FriendManager::getFriendInfo(FriendInfo::HumanKind friendKind, const std::string& friendCode, std::shared_ptr<FriendInfo>& friendInfo)
{
    int ret = ErrCode::InvalidArgument;

    if(friendKind == FriendInfo::HumanKind::Did) {
        ret = getFriendInfoByDid(friendCode, friendInfo);
    } else if(friendKind == FriendInfo::HumanKind::Ela) {
        ret = getFriendInfoByEla(friendCode, friendInfo);
    } else if(friendKind == FriendInfo::HumanKind::Carrier) {
        ret = getFriendInfoByCarrier(friendCode, friendInfo);
    }

    return ret;
}

std::vector<FriendInfo> FriendManager::filterFriends(std::string regex)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::acceptFriend(std::shared_ptr<FriendInfo> friendInfo)
{
    Log::I(Log::TAG, "==== %s", __PRETTY_FUNCTION__);
    auto msgMgr = SAFE_GET_PTR(mMessageManager);

    std::vector<FriendInfo::CarrierInfo> carrierInfoArray;
    int ret = friendInfo->getAllCarrierInfo(carrierInfoArray);
    if(ret < 0) {
        return ret;
    }

    for(const auto& info: carrierInfoArray) {
        FriendInfo::Status status;
        ret = friendInfo->getCarrierStatus(info.mUsrId, status);
        if(ret < 0) {
            return ret;
        }

        if(status != FriendInfo::Status::WaitForAccept) {
            continue;
        }

        ret = addFriendByCarrier(info.mUsrId, "", false);
        if(ret < 0) {
            return ret;
        }

        ret = friendInfo->setCarrierStatus(info.mUsrId, FriendInfo::Status::Offline);
        if(ret < 0) {
            return ret;
        }
    }

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
int FriendManager::addFriendByDid(const std::string& did, const std::string& summary, bool remoteRequest)
{
    //if(::isDidValid(did) == false) {
        //return ErrCode::InvalidArgument;
    //}
    //
    //auto msgMgr = SAFE_GET_PTR(mMessageManager);
    //int ret = msgMgr->requestFriend(did, 

    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::addFriendByCarrier(const std::string& carrierAddress, const std::string& summary, bool remoteRequest)
{
    auto msgMgr = SAFE_GET_PTR(mMessageManager);
    int ret = msgMgr->requestFriend(carrierAddress, MessageManager::ChannelType::Carrier, summary, remoteRequest);
    if(ret < 0) {
        return ret;
    }

    std::shared_ptr<FriendInfo> friendInfo;
    ret = tryGetFriendInfo(carrierAddress, friendInfo);
    if(ret < 0) { // not found
        friendInfo = std::make_shared<FriendInfo>(weak_from_this());
        mFriendList.push_back(friendInfo);
        FriendInfo::CarrierInfo carrierInfo = {
            .mUsrAddr = carrierAddress,
            .mUsrId = "",
            .mDevInfo = {"", "", 0},
        };
        ret = friendInfo->addCarrierInfo(carrierInfo, FriendInfo::Status::WaitForAccept);
        if(ret < 0) {
            return ret;
        }
    }

    return 0;
}

int FriendManager::addFriendByEla(const std::string& elaAddress, const std::string& summary, bool remoteRequest)
{
    if(::isAddressValid(elaAddress.c_str()) == false) {
        return ErrCode::InvalidArgument;
    }

    auto msgMgr = SAFE_GET_PTR(mMessageManager);
    int ret = msgMgr->requestFriend(elaAddress, MessageManager::ChannelType::ElaChain, summary);
    if(ret < 0) {
        return ret;
    }

    return 0;
}

int FriendManager::removeFriendByDid(const std::string& did)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::removeFriendByCarrier(const std::string& carrierAddress)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::removeFriendByEla(const std::string& elaAddress)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int FriendManager::getFriendInfoByDid(const std::string& did, std::shared_ptr<FriendInfo>& friendInfo)
{
    for(auto idx = 0; idx < mFriendList.size(); idx++) {
        std::string value;
        mFriendList[idx]->getHumanInfo(HumanInfo::Item::Did, value);

        if(value == did) {
            friendInfo = mFriendList[idx];
            return idx;
        }
    }

    return ErrCode::NotFoundError;
}

int FriendManager::getFriendInfoByCarrier(const std::string& carrierUsrId, std::shared_ptr<FriendInfo>& friendInfo)
{
    friendInfo.reset();

    for(auto idx = 0; idx < mFriendList.size(); idx++) {
        auto info = mFriendList[idx];
        std::vector<FriendInfo::CarrierInfo> carrierInfoArray;
        int ret = info->getAllCarrierInfo(carrierInfoArray);
        if(ret < 0) {
            return ret;
        }

        for(const auto& carrierInfo: carrierInfoArray) {
            if(carrierInfo.mUsrAddr == carrierUsrId
            || carrierInfo.mUsrId == carrierUsrId) {
                friendInfo = info;
                return idx;
            }
        }
    }

    return ErrCode::NotFoundError;
}

int FriendManager::getFriendInfoByEla(const std::string& elaAddress, std::shared_ptr<FriendInfo>& friendInfo)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}



} // namespace elastos
