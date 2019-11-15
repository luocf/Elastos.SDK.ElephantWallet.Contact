/**
 * @file	ElephantContact.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_P2P_NETWORK_BRIDGE_HPP_
#define _ELASTOS_P2P_NETWORK_BRIDGE_HPP_

#include <string>
#include "P2PNetworkBase.hpp"

namespace elastos {
    class P2PNetworkBridge {
    public:
        ~P2PNetworkBridge();
        std::shared_ptr <elastos::P2PNetworkBase> GetP2PNetwork();
        void SetP2PNetwork(std::shared_ptr <elastos::P2PNetworkBase> p2p_network);

        P2PNetworkBridge(const P2PNetworkBridge&)=delete;
        P2PNetworkBridge& operator=(const P2PNetworkBridge&)=delete;
        static P2PNetworkBridge& getInstance(){//通过局部静态变量的特性保证了线程安全
            static P2PNetworkBridge instance;
            return instance;
        }
    private:
        P2PNetworkBridge() {}
        std::shared_ptr <elastos::P2PNetworkBase> mP2PNetwork;
    }; // class P2PNetworkBridge
}


#endif /* _ELASTOS_P2P_NETWORK_HPP_ */

