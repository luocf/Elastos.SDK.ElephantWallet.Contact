#ifndef _ELASTOS_CONFIG_HPP_
#define _ELASTOS_CONFIG_HPP_

#include <map>
#include <string>
#include <vector>

#include "ErrCode.hpp"

namespace elastos {

class Config {
public:
    /*** type define ***/
    struct CarrierConfig {
        struct BootstrapNode {
            const std::string mIpv4;
            const std::string mPort;
            const std::string mPublicKey;
        };

        int mLogLevel;
        bool mEnableUdp;
        std::vector<BootstrapNode> mBootstrapNodes;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit Config(const std::string& cfgFilePath);
    virtual ~Config();

    int load();
    int save();

    std::string mUserDataDir;
    std::unique_ptr<CarrierConfig> mCarrierConfig;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::string mConfigFilePath;

}; // class Config

} // namespace elastos

#endif /* _ELASTOS_CONFIG_HPP_ */
