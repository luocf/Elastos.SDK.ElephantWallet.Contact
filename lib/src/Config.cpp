//
//  Config.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <Config.hpp>

#include "misc/CompatibleFileSystem.hpp"

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
Config::Config(const std::string& cfgFilePath)
    : mUserDataDir()
    , mCarrierConfig()
    , mConfigFilePath(cfgFilePath)
{
    mUserDataDir = std::filesystem::path(mConfigFilePath).parent_path().string();
}

Config::~Config()
{
}

int Config::load()
{
    mCarrierConfig = std::make_unique<CarrierConfig>();
    mCarrierConfig->mLogLevel = 4;
    mCarrierConfig->mEnableUdp = true;
    mCarrierConfig->mBootstrapNodes.push_back({ "13.58.208.50",  "33445", "89vny8MrKdDKs7Uta9RdVmspPjnRMdwMmaiEW27pZ7gh" });
    mCarrierConfig->mBootstrapNodes.push_back({ "18.216.6.197",  "33445", "H8sqhRrQuJZ6iLtP2wanxt4LzdNrN2NNFnpPdq1uJ9n2" });
    mCarrierConfig->mBootstrapNodes.push_back({ "18.216.102.47", "33445", "G5z8MqiNDFTadFUPfMdYsYtkUDbX5mNCMVHMZtsCnFeb" });
    mCarrierConfig->mBootstrapNodes.push_back({ "52.83.127.85",  "33445", "CDkze7mJpSuFAUq6byoLmteyGYMeJ6taXxWoVvDMexWC" });
    mCarrierConfig->mBootstrapNodes.push_back({ "52.83.127.216", "33445", "4sL3ZEriqW7pdoqHSoYXfkc1NMNpiMz7irHMMrMjp9CM" });
    mCarrierConfig->mBootstrapNodes.push_back({ "52.83.171.135", "33445", "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL" });

    return 0;
}

int Config::save()
{
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Unimplemented!!!");
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
