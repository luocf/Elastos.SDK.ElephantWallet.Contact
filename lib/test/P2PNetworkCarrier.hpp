/**
 * @file	ElephantContact.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_P2P_NETWORK_CARRIER_HPP_
#define _ELASTOS_P2P_NETWORK_CARRIER_HPP_

#include <string>
#include <sstream>
#include "MicroService.hpp"
#include "P2PNetworkBase.hpp"

namespace elastos {
    class P2PNetworkCarrier : public P2PNetworkBase {
    public:
        P2PNetworkCarrier();

        ~P2PNetworkCarrier();

        void Create(const std::string &pub_key, const std::string &dir) override;

        void Start() override;

        void Stop() override;

        int GetHumanInfo(const char *humanCode, std::stringstream *info) override;

        int GetHumanStatus(const char *humanCode) override;

        std::string GetPubKey();

        static P2PNetworkCarrier* sP2PNetworkCarrier;
    private:
        std::string mPubKey;
        std::string mDir;
    }; // class P2PNetworkCarrier
}


#endif /* _ELASTOS_P2P_NETWORK_HPP_ */

