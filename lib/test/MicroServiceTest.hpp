//
// Created by luocf on 2019-11-14.
//

#ifndef ELASTOS_SDK_CONTACT_MICROSERVICETEST_HPP
#define ELASTOS_SDK_CONTACT_MICROSERVICETEST_HPP
#include <string>
#include <sstream>
#include "MicroService.hpp"
#include "P2PNetworkBase.hpp"
namespace elastos {
    static const char *MicroServiceTest_TAG = "_MicroServiceTest_";
    class MicroServiceTest  :public std::enable_shared_from_this<MicroServiceTest>, public elastos::MicroService {
    public:
        MicroServiceTest();
        ~MicroServiceTest();
        void Create(const std::string &pub_key, const std::string &dir) override ;
        void Start() override ;
        void Stop() override ;
        void HandleMessage(const std::string &humanCode, const std::string &msg) override;

    private:
        std::shared_ptr<elastos::P2PNetworkBase> mP2PNetworkBase;
    }; // class MicroService
}
#endif //ELASTOS_SDK_CONTACT_MICROSERVICETEST_HPP
