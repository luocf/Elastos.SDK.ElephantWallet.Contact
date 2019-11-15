/**
 * @file	ElephantContact.hpp
 * @brief	Contact
 * @details
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_P2P_NETWORK_BASE_HPP_
#define _ELASTOS_P2P_NETWORK_BASE_HPP_

#include <string>
#include <sstream>
#include "MicroService.hpp"
#include <ElephantContact.hpp>
namespace elastos {
    static const char *P2PNetwork_TAG = "_P2PNetwork_";
    class P2PNetworkBase {
    public:
        virtual void Create(const std::string &pub_key, const std::string &dir) = 0;

        virtual void Start() = 0;

        virtual void Stop() = 0;

        virtual int GetHumanInfo(const char *humanCode, std::stringstream *info) = 0;

        virtual int GetHumanStatus(const char *humanCode) = 0;

        void bindMicrService(MicroService* microService);

        void onReceivedMessage(const std::string &humanCode, const std::string &msgInfo);

        virtual ~P2PNetworkBase();

        P2PNetworkBase();

    protected:
        MicroService* mMicroService;
    }; // class P2PNetworkBase
}

#endif /* _ELASTOS_P2P_NETWORK_HPP_ */

