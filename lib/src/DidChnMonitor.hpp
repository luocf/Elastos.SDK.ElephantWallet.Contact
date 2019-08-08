#ifndef _DIDCHN_MONITOR_HPP_
#define _DIDCHN_MONITOR_HPP_


#include <map>
#include <mutex>
#include <set>
#include <Config.hpp>
#include <HumanInfo.hpp>
#include <Json.hpp>
#include <SecurityManager.hpp>
#include <ThreadPool.hpp>

namespace elastos {

class DidChnMonitor {
public:
    /*** type define ***/
    class MonitorCallback {
    public:
        virtual void onError(const std::string& did, const std::string& key, int errcode)  = 0;
        virtual void onChanged(const std::string& did, const std::string& key, const std::vector<std::string>& didProps)  = 0;

    private:
        bool mWithFriendId;
        friend DidChnMonitor;
    };

    /*** static function and variable ***/
    static int InitInstance(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr);
    static std::shared_ptr<DidChnMonitor> GetInstance();

    static constexpr const char* NamePublicKey = "PublicKey";
    static constexpr const char* NameCarrierId = "CarrierID";
    static constexpr const char* NameFriendId  = "FriendID";

    /*** class function and variable ***/
    int setConnectTimeout(uint32_t milliSecond);

    int appendMoniter(const std::string& did, std::shared_ptr<MonitorCallback> callback, bool withFriendId);
    int removeMoniter(const std::string& did);

    int cacheDidProp(const std::string& key, const std::string& value);
    int uploadCachedDidProp();
    int printCachedDidProp(std::string& output);

    int startMonitor();

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
        constexpr static uint32_t MonitorPendingMS = 30000;

        ThreadPool mMonitorThread;
        std::map<std::string, std::shared_ptr<MonitorCallback>> mMonitorCallbackMap;

        std::function<void()> mMonitorLooper;
    };

    /*** static function and variable ***/
    static std::shared_ptr<DidChnMonitor> gDidChnMonitor;
    static constexpr const char* DataFileName = "cacheddata.dat";

    /*** class function and variable ***/
    explicit DidChnMonitor(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~DidChnMonitor();

    int uploadDidPropsByAgent(const std::vector<std::pair<std::string, std::string>>& didProps);
    int serializeDidProps(const std::vector<std::pair<std::string, std::string>>& didProps, std::string& result);
    int makeDidAgentData(const std::string& didProtocolData, std::string& result);
    int uploadDidAgentData(const std::string& didAgentData);

    int checkDidProps(const std::string& did, std::shared_ptr<MonitorCallback> callback);
    int checkDidProps(const std::string& did, const std::string& key, const std::vector<std::string>& didProps);

    int downloadDidPropsByAgent(const std::string& did, const std::string& key, bool withHistory,
                                std::vector<std::string>& values);
    int downloadDidChnData(const std::string& path, std::string& result);

    int getDidPropPath(const std::string& did, const std::string& key, bool withHistory, std::string& path);

    void refreshUpdateTime(const std::string& did, const std::string& key);
    bool checkUpdateTime(const std::string& did, const std::string& key, int64_t updateTime);
    int clearDidPropCache(bool refreshUpdateTime);

    int loadLocalData();
    int saveLocalData();

    int getPropKeyPathPrefix(std::string& keyPathPrefix);
    int getPropKeyPath(const std::string& key, std::string& keyPath);

    std::weak_ptr<Config> mConfig;
    std::weak_ptr<SecurityManager> mSecurityManager;
    uint32_t mConnectTimeoutMS;

    std::recursive_mutex mMutex;
    std::string mPropKeyPathPrefix;
    std::vector<std::pair<std::string, std::string>> mDidPropCache;
    std::map<std::string, int64_t> mDidPropUpdateTime;
    Monitor mMonitor;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

} // namespace elastos

#endif /* _DIDCHN_MONITOR_HPP_ */

