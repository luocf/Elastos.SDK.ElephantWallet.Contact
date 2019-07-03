#ifndef _BLKCHN_CLIENT_HPP_
#define _BLKCHN_CLIENT_HPP_


#include <map>
#include <mutex>
#include <set>
#include <Config.hpp>
#include <Json.hpp>
#include <HumanInfo.hpp>
#include <SecurityManager.hpp>
#include <ThreadPool.hpp>

namespace elastos {

class BlkChnClient {
public:
    /*** type define ***/
    using MonitorCallback = std::function<void(int errcode, const std::string& keyPath, const std::string& result)>;

    /*** static function and variable ***/
    static int InitInstance(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr);
    static std::shared_ptr<BlkChnClient> GetInstance();

    static constexpr const char* NamePublicKey = "PublicKey";
    static constexpr const char* NameCarrierId = "CarrierID";

    /*** class function and variable ***/
    int setConnectTimeout(uint32_t milliSecond);

    int appendMoniter(const std::string& path, const MonitorCallback& callback);
    int removeMoniter(const std::string& path);

    int downloadAllDidProps(const std::string& did, std::map<std::string, std::string>& propMap);
    int uploadAllDidProps(const std::map<std::string, std::string>& propMap);

    int downloadDidProp(const std::string& did, const std::string& key, std::string& prop);
    int uploadDidProp(const std::string& key, const std::string& prop);

    //int getDidProp(const std::string& did, const std::string& key, std::string& value);
    int getDidPropHistory(const std::string& did, const std::string& key, std::vector<std::string>& values);
    int getDidPropHistoryPath(const std::string& did, const std::string& key, std::string& path);

    int downloadHumanInfo(const std::string& did, std::shared_ptr<HumanInfo>& humanInfo);
    int uploadHumanInfo(const std::shared_ptr<HumanInfo>& humanInfo);

protected:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/

private:
    /*** type define ***/
    struct DidProtocol {
        struct Name {
            static constexpr const char* Tag = "Tag";
            static constexpr const char* Ver = "Ver";
            static constexpr const char* Properties = "Properties";
            static constexpr const char* Key = "Key";
            static constexpr const char* Value = "Value";
            static constexpr const char* Status = "Status";
        };

        struct Value {
            static constexpr const char* Tag = "DID Property";
            static constexpr const char* Ver = "1.0";
            struct Status {
                static constexpr const char* Normal = "Normal";
                static constexpr const char* Deprecated = "Deprecated";
            };
        };
    };

    class Monitor {
    public:
        constexpr static uint32_t mMonitorPendingMS = 10000;

        ThreadPool mMonitorThread;
        std::mutex mMonitorMutex;
        std::map<std::string, MonitorCallback> mMonitorCallbackMap;

        std::function<void()> mMonitorLooper;
    };

    /*** static function and variable ***/
    static std::shared_ptr<BlkChnClient> gBlkChnClient;

    /*** class function and variable ***/
    explicit BlkChnClient(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~BlkChnClient();
    int startMonitor();

    int getDidPropFromDidChn(const std::string& path, std::string& result);

    int getPropKeyPathPrefix(std::string& keyPathPrefix);
    int getPropKeyPath(const std::string& key, std::string& keyPath);

    std::weak_ptr<Config> mConfig;
    std::weak_ptr<SecurityManager> mSecurityManager;
    uint32_t mConnectTimeoutMS;

    Monitor mMonitor;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

} // namespace elastos

#endif /* _BLKCHN_CLIENT_HPP_ */

