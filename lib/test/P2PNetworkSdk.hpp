/**
 * @file	ElephantContact.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_P2P_NETWORK_SDK_HPP_
#define _ELASTOS_P2P_NETWORK_SDK_HPP_

#include <string>
#include <sstream>
#include <stdlib.h>
#include "MicroService.hpp"
#include "P2PNetworkBase.hpp"
#include <ElephantContact.hpp>

namespace elastos {
    class P2PNetworkSdk : public elastos::sdk::ElephantContact, public P2PNetworkBase {
    public:
        P2PNetworkSdk();

        ~P2PNetworkSdk();

        void Create(const std::string &pub_key, const std::string &dir) override;

        void Start() override;

        void Stop() override;

        int GetHumanInfo(const char *humanCode, std::stringstream *info) override;

        int GetHumanStatus(const char *humanCode) override;

        std::string GetPubKey();
        void AcceptFriend(const char* friendCode);
        static P2PNetworkSdk* sP2PNetworkSdk;
    private:
        std::string mPubKey;
        std::string mDir;
        std::shared_ptr<elastos::sdk::ElephantContact> mElephantContact;
    }; // class P2PNetworkSdk
}


#endif /* _ELASTOS_P2P_NETWORK_HPP_ */

