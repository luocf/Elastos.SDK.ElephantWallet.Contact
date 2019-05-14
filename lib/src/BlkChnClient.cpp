#include <BlkChnClient.hpp>

#include <HttpClient.hpp>
#include <Log.hpp>


namespace elastos {

/* =========================================== */
/* === static variables initialize =========== */
/* =========================================== */
std::shared_ptr<BlkChnClient> BlkChnClient::gBlkChnClient {};

/* =========================================== */
/* === static function implement ============= */
/* =========================================== */
int BlkChnClient::InitInstance(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr)
{
    if(gBlkChnClient.get() != nullptr) {
        return 0;
    }

    struct Impl: BlkChnClient {
        Impl(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr) : BlkChnClient(config, sectyMgr) {
        }
    };

    HttpClient::InitGlobal();
    gBlkChnClient = std::make_shared<Impl>(config, sectyMgr);

    return 0;
}

std::shared_ptr<BlkChnClient> BlkChnClient::GetInstance()
{
    assert(gBlkChnClient.get() != nullptr);
    return gBlkChnClient;
}

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */
int BlkChnClient::setConnectTimeout(uint32_t milliSecond)
{
    mConnectTimeoutMS = milliSecond;
    return 0;
}

int BlkChnClient::getDidProps(const std::set<std::string>& keySet, std::map<std::string, std::string>& propMap)
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

int BlkChnClient::uploadDidProps(const std::map<std::string, std::string>& propMap)
{
    Json jsonPropProt = Json::object();
    Json jsonPropArray = Json::array();

    for(const auto& prop: propMap) {
        Json jsonProp = Json::object();
        jsonProp[prop.first] = prop.second;
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

    auto propProtStr = std::make_shared<std::string>(jsonPropProt.dump());
    // TODO, did prop key, sign, make {msg, sig, pub}

    auto task = [propProtStr]() {
        Log::D(Log::TAG, "%s body = %s", __PRETTY_FUNCTION__, propProtStr->c_str());

        HttpClient httpClient;
        // TODO

    };
    mTaskThread.post(task);

    return 0;
}

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */
BlkChnClient::BlkChnClient(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr)
    : mConfig(config)
    , mSecurityManager(sectyMgr)
    , mTaskThread()
    , mConnectTimeoutMS(10000)
{
}

BlkChnClient::~BlkChnClient()
{
}

} // namespace elastos
