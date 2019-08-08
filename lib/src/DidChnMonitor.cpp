#include <DidChnMonitor.hpp>

#include <iomanip>

#include <CompatibleFileSystem.hpp>
#include <DateTime.hpp>
#include <ErrCode.hpp>
#include <HttpClient.hpp>
#include <Log.hpp>
#include <MD5.hpp>
#include <Platform.hpp>
#include <SafePtr.hpp>
#include "JsonDefine.hpp"
#include "DidChnMonitor.hpp"
#include "../include/ErrCode.hpp"


namespace elastos {

/* =========================================== */
/* === static variables initialize =========== */
/* =========================================== */
std::shared_ptr<DidChnMonitor> DidChnMonitor::gDidChnMonitor {};

/* =========================================== */
/* === static function implement ============= */
/* =========================================== */
int DidChnMonitor::InitInstance(std::weak_ptr<Config> config,
                               std::weak_ptr<SecurityManager> sectyMgr) {
    if (gDidChnMonitor.get() != nullptr) {
        return 0;
    }

    struct Impl : DidChnMonitor {
        Impl(std::weak_ptr<Config> config,
             std::weak_ptr<SecurityManager> sectyMgr)
            : DidChnMonitor(config, sectyMgr) {}
    };

    HttpClient::InitGlobal();
    gDidChnMonitor = std::make_shared<Impl>(config, sectyMgr);

    int ret = gDidChnMonitor->loadLocalData();
    if(ret == ErrCode::FileNotExistsError) {
        Log::D(Log::TAG, "DidChnMonitor::InitInstance() Local data file is not exists.");
        return 0;
    }
    CHECK_ERROR(ret);

    return 0;
}

std::shared_ptr<DidChnMonitor> DidChnMonitor::GetInstance()
{
    assert(gDidChnMonitor.get() != nullptr);
    return gDidChnMonitor;
}

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */
int DidChnMonitor::setConnectTimeout(uint32_t milliSecond)
{
    mConnectTimeoutMS = milliSecond;
    return 0;
}

int DidChnMonitor::appendMoniter(const std::string& did, std::shared_ptr<MonitorCallback> callback, bool withFriendId)
{
    if(did.empty() == true) {
        return ErrCode::InvalidArgument;
    }

	std::lock_guard<std::recursive_mutex> lock(mMutex);
    callback->mWithFriendId = withFriendId;
    mMonitor.mMonitorCallbackMap[did] = callback;

    return 0;
}

int DidChnMonitor::removeMoniter(const std::string& did)
{
    if(did.empty() == true) {
        return ErrCode::InvalidArgument;
    }

	std::lock_guard<std::recursive_mutex> lock(mMutex);
    mMonitor.mMonitorCallbackMap.erase(did);

    return 0;
}

int DidChnMonitor::cacheDidProp(const std::string& key, const std::string& value)
{
    Log::I(Log::TAG, "DidChnMonitor::cacheDidProp() key=%s, value=%s", key.c_str(), value.c_str());

    std::lock_guard<std::recursive_mutex> lock(mMutex);

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::string did;
    int ret = sectyMgr->getDid(did);
    CHECK_ERROR(ret)

    refreshUpdateTime(did, key);
    mDidPropCache.push_back({key, value});

    ret = saveLocalData();
    CHECK_ERROR(ret);

    return 0;
}

int DidChnMonitor::uploadCachedDidProp()
{
    std::lock_guard<std::recursive_mutex> lock(mMutex);

    int ret = uploadDidPropsByAgent(mDidPropCache);
    CHECK_ERROR(ret)

    ret = clearDidPropCache(true);
    CHECK_ERROR(ret)

    return 0;
}

int DidChnMonitor::printCachedDidProp(std::string& output)
{
    std::lock_guard<std::recursive_mutex> lock(mMutex);

    try {
        Json jsonCache;
        jsonCache[JsonKey::DidPropCache] = mDidPropCache;
        jsonCache[JsonKey::UpdateTime] = mDidPropUpdateTime;
        output = jsonCache.dump(2);
    } catch(const std::exception& ex) {
        Log::E(Log::TAG, "Failed to print cached did prop.\nex=%s", ex.what());
        return ErrCode::JsonParseException;
    }

    return 0;
}

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */
DidChnMonitor::DidChnMonitor(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr)
    : mConfig(config)
    , mSecurityManager(sectyMgr)
    , mConnectTimeoutMS(10000)
    , mMutex()
    , mPropKeyPathPrefix()
    , mDidPropCache()
    , mMonitor()
{
}

DidChnMonitor::~DidChnMonitor()
{
}

int DidChnMonitor::startMonitor()
{
    mMonitor.mMonitorLooper = [&]() {
        int64_t current = DateTime::CurrentMS();
        Log::I(Log::TAG, "%s current timestamp=%lld", __PRETTY_FUNCTION__, current);
        std::map<std::string, std::shared_ptr<MonitorCallback>> monitorCallbackMap;
        {
            std::lock_guard<std::recursive_mutex> lock(mMutex);
            monitorCallbackMap = mMonitor.mMonitorCallbackMap;
        }

        for(const auto& it: monitorCallbackMap) {
            auto& did = it.first;
            auto& callback = it.second;

            std::ignore = DidChnMonitor::checkDidProps(did, callback);
        }

        int ret = mMonitor.mMonitorThread.sleepMS(mMonitor.MonitorPendingMS);
        mMonitor.mMonitorThread.post(mMonitor.mMonitorLooper);
    };

    mMonitor.mMonitorThread.post(mMonitor.mMonitorLooper);

    return 0;
}

void DidChnMonitor::refreshUpdateTime(const std::string& did, const std::string& key)
{
    std::string updateTimeKey = (did + "-" + key);

    mDidPropUpdateTime.insert({updateTimeKey, DateTime::CurrentMS()});
}

bool DidChnMonitor::checkUpdateTime(const std::string& did, const std::string& key, int64_t updateTime)
{
    std::string updateTimeKey = (did + "-" + key);

    if(mDidPropUpdateTime.find(updateTimeKey) == mDidPropUpdateTime.end()) {
        return true;
    }

    int64_t lastUpdateTime = mDidPropUpdateTime[updateTimeKey];
    if(lastUpdateTime < updateTime) {
        return true;
    }

    return false;
}

int DidChnMonitor::uploadDidPropsByAgent(const std::vector<std::pair<std::string, std::string>>& didProps)
{
    if(didProps.size() == 0) {
        Log::I(Log::TAG, "DidChnMonitor::uploadCachedDidProp() Ignore to update empty cache.");
        return 0;
    }

    std::string didProtocolData;
    int ret = serializeDidProps(didProps, didProtocolData);
    CHECK_ERROR(ret)

    std::string didAgentData;
    ret = makeDidAgentData(didProtocolData, didAgentData);
    CHECK_ERROR(ret)

    ret = uploadDidAgentData(didAgentData);
    CHECK_ERROR(ret)

    return 0;
}

int DidChnMonitor::serializeDidProps(const std::vector<std::pair<std::string, std::string>>& didProps,
                                     std::string& result)
{
    if(didProps.size() == 0) {
        return ErrCode::BlkChnEmptyPropError;
    }

    Json jsonPropProt = Json::object();
    Json jsonPropArray = Json::array();
    for(const auto& prop: didProps) {
        std::string keyPath;
        int ret = getPropKeyPath(prop.first, keyPath);
        if (ret < 0) {
            return ret;
        }

        std::string propKey = keyPath;
        std::string propValue = prop.second;
        Json jsonProp = Json::object();
        jsonProp[DidProtocol::Name::Key] = propKey;
        jsonProp[DidProtocol::Name::Value] = propValue;
        if(prop.second.empty() == true) {
            jsonProp[DidProtocol::Name::Status] = DidProtocol::Value::Status::Deprecated;
        } else {
            jsonProp[DidProtocol::Name::Status] = DidProtocol::Value::Status::Normal;
        }

        jsonPropArray.push_back(jsonProp);
    }

    jsonPropProt[DidProtocol::Name::Tag] = DidProtocol::Value::Tag;
    jsonPropProt[DidProtocol::Name::Ver] = DidProtocol::Value::Ver;
    jsonPropProt[DidProtocol::Name::Status] = DidProtocol::Value::Status::Normal;
    jsonPropProt[DidProtocol::Name::Properties] = jsonPropArray;

    result = jsonPropProt.dump();

    return 0;
}

int DidChnMonitor::makeDidAgentData(const std::string& didProtocolData, std::string& result)
{
    if(didProtocolData.empty() == true) {
        return ErrCode::BlkChnEmptyPropError;
    }

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);

    std::vector<uint8_t> originBytes(didProtocolData.begin(), didProtocolData.end());
    std::vector<uint8_t> signedBytes;
    int ret = sectyMgr->signData(originBytes, signedBytes);
    CHECK_ERROR(ret)

    std::string pubKey;
    ret = sectyMgr->getPublicKey(pubKey);
    CHECK_ERROR(ret)

    std::string msgStr = MD5::MakeHexString(originBytes);
    std::string sigStr = MD5::MakeHexString(signedBytes);

    // did prop key, sign, make {msg, sig, pub}
    result = std::string("{")
                          + "\"pub\":\"" + pubKey + "\","
                          + "\"msg\":\"" + msgStr + "\","
                          + "\"sig\":\"" + sigStr + "\""
                          + "}";
    return 0;
}

int DidChnMonitor::uploadDidAgentData(const std::string& didAgentData)
{
    if(didAgentData.empty() == true) {
        return ErrCode::BlkChnEmptyPropError;
    }

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    auto config = SAFE_GET_PTR(mConfig);

    auto didConfigUrl = config->mDidChainConfig->mUrl;
    auto agentUploadPath = config->mDidChainConfig->mAgentApi.mUploadDidProps;
    std::string agentUploadUrl = didConfigUrl + agentUploadPath;
    std::string authHeader;
    int ret = sectyMgr->getDidAgentAuthHeader(authHeader);
    CHECK_ERROR(ret)

    const auto& reqBody = didAgentData;
    Log::I(Log::TAG, "DidChnMonitor::uploadDidAgentData() reqBody=%s", reqBody.c_str());

    HttpClient httpClient;
    httpClient.url(agentUploadUrl);
    httpClient.setHeader("Content-Type", "application/json");
    httpClient.setHeader("X-Elastos-Agent-Auth", authHeader);
    ret = httpClient.syncPost(reqBody);
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }

    std::string respBody;
    ret = httpClient.getResponseBody(respBody);
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }
    Log::I(Log::TAG, "DidChnMonitor::uploadDidAgentData() respBody=%s", respBody.c_str());

    Json jsonResp = Json::parse(respBody);
    if(jsonResp["status"] != 200) {
        return ErrCode::BlkChnSetPropError;
    }

    std::string txid = jsonResp["result"];
    if(txid.empty() == true) {
        return ErrCode::BlkChnBadTxIdError;
    }

    return 0;
}

int DidChnMonitor::checkDidProps(const std::string& did, std::shared_ptr<MonitorCallback> callback)
{
    if(callback.get() == nullptr) {
        return ErrCode::InvalidArgument;
    }

    const struct {
        const char* key;
        bool withHistory;
    } checkPropKeys [] = {
            { NameCarrierId, true },
            { NameFriendId, true },
    };

    for(const auto& it: checkPropKeys) {
        if(callback->mWithFriendId == false
        && it.key == NameFriendId) {
            continue;
        }

        std::string key = it.key;

        std::vector<std::string> didProps;
        int ret = downloadDidPropsByAgent(did, key, it.withHistory, didProps);
        if(ret < 0) {
            Log::V(Log::TAG, "DidChnMonitor::checkDidProps() Failed to check %s: %s", did.c_str(), key.c_str());
            callback->onError(did, key, ret);
            continue;
        }

        ret  = checkDidProps(did, key, didProps);
        if(ret < 0) {
            callback->onError(did, key, ret);
            continue;
        }
        if(ret == 0) { // not changed
            continue;
        }

        refreshUpdateTime(did, key);
        callback->onChanged(did, key, didProps);
    }

    return 0;
}

int DidChnMonitor::checkDidProps(const std::string& did, const std::string& key, const std::vector<std::string>& didProps)
{
    int64_t lastUpdateTime = -1;
    for(const auto& prop: didProps) {
        Json jsonProp = Json::parse(prop);
        if(jsonProp.find(JsonKey::UpdateTime) == jsonProp.end()) {
            Log::V(Log::TAG, "DidChnMonitor::checkDidProps() Failed to get updatetime from %s: %s -> %s", did.c_str(), key.c_str(), prop.c_str());
            return ErrCode::BlkChnBadUpdateTimeError;
        }

        int64_t updateTime = jsonProp[JsonKey::UpdateTime];
        if(lastUpdateTime < updateTime) {
            lastUpdateTime = updateTime;
        }
    }

    bool changed = checkUpdateTime(did, key, lastUpdateTime);
    Log::V(Log::TAG, "DidChnMonitor::checkDidProps() %s: %s changed=%d.", did.c_str(), key.c_str(), changed);

    return (changed == true ? 1 : 0);
}

int DidChnMonitor::downloadDidPropsByAgent(const std::string& did, const std::string& key, bool withHistory,
                                           std::vector<std::string>& values)
{

    std::string dataPath;
    int ret = getDidPropPath(did, key, withHistory, dataPath);
    CHECK_ERROR(ret)

    std::string propArrayStr;
    ret = downloadDidChnData(dataPath, propArrayStr);
    CHECK_ERROR(ret)

    Json jsonPropArray = Json::parse(propArrayStr);
    for(const auto& it: jsonPropArray) {
        values.push_back(it["value"]);
    }

    return 0;
}

int DidChnMonitor::downloadDidChnData(const std::string& path, std::string& result)
{
    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    auto config = SAFE_GET_PTR(mConfig);

    auto didConfigUrl = config->mDidChainConfig->mUrl;
    std::string agentUrl = didConfigUrl + path;

    HttpClient httpClient;
    httpClient.url(agentUrl);
    int ret = httpClient.syncGet();
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }

    std::string respBody;
    ret = httpClient.getResponseBody(respBody);
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }
    Log::I(Log::TAG, "respBody=%s", respBody.c_str());

    Json jsonResp = Json::parse(respBody);
    int status = jsonResp["status"];
    if(status != 200) {
        return ErrCode::BlkChnGetPropError;
    }

    result = jsonResp["result"];
    if(result.empty() == true) {
        return ErrCode::BlkChnEmptyPropError;
    }

    return 0;
}

int DidChnMonitor::getDidPropPath(const std::string& did, const std::string& key, bool withHistory,
                                  std::string& path)
{
    path.clear();

    auto config = SAFE_GET_PTR(mConfig);

    std::string keyPath;
    int ret = getPropKeyPath(key, keyPath);
    CHECK_ERROR(ret)

    auto agentGetProps = config->mDidChainConfig->mAgentApi.mGetDidProps;
    auto agentDidPropApi = (withHistory == true
                            ? config->mDidChainConfig->mAgentApi.mDidPropHistory
                            : config->mDidChainConfig->mAgentApi.mDidProp);
    path = agentGetProps + did + agentDidPropApi + keyPath;

    return 0;
}

int DidChnMonitor::clearDidPropCache(bool refreshUpdateTime)
{
    Log::I(Log::TAG, "DidChnMonitor::clearDidPropCache() refreshUpdateTime=%d", refreshUpdateTime);

    std::lock_guard<std::recursive_mutex> lock(mMutex);

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::string did;
    int ret = sectyMgr->getDid(did);
    CHECK_ERROR(ret)

    if(refreshUpdateTime == true) {
        for (const auto &it: mDidPropCache) {
            const auto &key = it.first;
            DidChnMonitor::refreshUpdateTime(did, key);
        }
    }

    mDidPropCache.clear();

    ret = saveLocalData();
    CHECK_ERROR(ret);

    return 0;
}

int DidChnMonitor::loadLocalData()
{
    auto config = SAFE_GET_PTR(mConfig);
    auto dataFilePath = elastos::filesystem::path(config->mUserDataDir.c_str()) / DataFileName;

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::vector<uint8_t> originData;
    int ret = sectyMgr->loadCryptoFile(dataFilePath.string(), originData);
    CHECK_ERROR(ret)

    std::string cacheData {originData.begin(), originData.end()};
    try {
        Json jsonCache = Json::parse(cacheData);

        mDidPropCache = jsonCache[JsonKey::DidPropCache].get<std::vector<std::pair<std::string, std::string>>>();
        mDidPropUpdateTime = jsonCache[JsonKey::UpdateTime].get<std::map<std::string, int64_t>>();
    } catch(const std::exception& ex) {
        Log::E(Log::TAG, "Failed to load local data from: %s.\nex=%s", dataFilePath.c_str(), ex.what());
        return ErrCode::JsonParseException;
    }

    Log::I(Log::TAG, "Success to load local data from: %s.", dataFilePath.c_str());
    return 0;
}

int DidChnMonitor::saveLocalData()
{
    auto config = SAFE_GET_PTR(mConfig);
    auto dataFilePath = elastos::filesystem::path(config->mUserDataDir.c_str()) / DataFileName;

    std::string cacheData;
    try {
        Json jsonCache;
        jsonCache[JsonKey::DidPropCache] = mDidPropCache;
        jsonCache[JsonKey::UpdateTime] = mDidPropUpdateTime;
        cacheData = jsonCache.dump();
    } catch(const std::exception& ex) {
        Log::E(Log::TAG, "Failed to save local data to: %s.\nex=%s", dataFilePath.c_str(), ex.what());
        return ErrCode::JsonParseException;
    }

    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    std::vector<uint8_t> originData {cacheData.begin(), cacheData.end()};
    int ret = sectyMgr->saveCryptoFile(dataFilePath.string(), originData);
    CHECK_ERROR(ret)

    Log::D(Log::TAG, "Save local data to: %s, data: %s", dataFilePath.c_str(), cacheData.c_str());

    return 0;
}

int DidChnMonitor::getPropKeyPathPrefix(std::string& keyPathPrefix)
{
    std::lock_guard<std::recursive_mutex> lock(mMutex);

    if(mPropKeyPathPrefix.empty() == true) {
        auto sectyMgr = SAFE_GET_PTR(mSecurityManager);

        std::string appId;
        int ret = sectyMgr->getDidPropAppId(appId);
        if (ret < 0) {
            return ret;
        }

        mPropKeyPathPrefix = "Apps/" + appId + "/";
    }

    keyPathPrefix = mPropKeyPathPrefix;

    return 0;
}

int DidChnMonitor::getPropKeyPath(const std::string& key, std::string& keyPath)
{
    if(key == "PublicKey") {
        keyPath = key;
        return 0;
    }

    std::string keyPathPrefix;
    int ret = getPropKeyPathPrefix(keyPathPrefix);
    CHECK_ERROR(ret)

    keyPath = (keyPathPrefix + key);
    return 0;
}

} // namespace elastos
