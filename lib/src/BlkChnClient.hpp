#ifndef _BLKCHN_CLIENT_HPP_
#define _BLKCHN_CLIENT_HPP_


#include <map>
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

    /*** static function and variable ***/
    static int InitInstance(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr);
    static std::shared_ptr<BlkChnClient> GetInstance();

    /*** class function and variable ***/
    int setConnectTimeout(uint32_t milliSecond);

    int downloadAllDidProps(const std::string& did, std::map<std::string, std::string>& propMap);
    int uploadAllDidProps(const std::map<std::string, std::string>& propMap);

    int downloadDidProp(const std::string& did, const std::string& key, std::string& prop);
    int uploadDidProp(const std::string& key, const std::string& prop);

    //int getDidProp(const std::string& did, const std::string& key, std::string& value);
    int getDidPropHistory(const std::string& did, const std::string& key, std::vector<std::string>& values);

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

    /*** static function and variable ***/
    static std::shared_ptr<BlkChnClient> gBlkChnClient;

    /*** class function and variable ***/
    explicit BlkChnClient(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr);
    virtual ~BlkChnClient();
    int getDidPropFromDidChn(const std::string& path, std::string& result);

    int getPropKeyPath(std::string& keyPath);

    std::weak_ptr<Config> mConfig;
    std::weak_ptr<SecurityManager> mSecurityManager;
    ThreadPool mTaskThread;
    uint32_t mConnectTimeoutMS;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

} // namespace elastos

#endif /* _BLKCHN_CLIENT_HPP_ */

