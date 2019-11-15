/**
 * @file	ElephantContact.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/
#include <string>
#include <sstream>
#include "P2PNetworkBridge.hpp"

namespace elastos {

    P2PNetworkBridge::~P2PNetworkBridge() {

    }

    std::shared_ptr <elastos::P2PNetworkBase> P2PNetworkBridge::GetP2PNetwork() {
        return mP2PNetwork;
    }

    void P2PNetworkBridge::SetP2PNetwork(std::shared_ptr <elastos::P2PNetworkBase> p2p_network) {
        mP2PNetwork = p2p_network;
    }
}


