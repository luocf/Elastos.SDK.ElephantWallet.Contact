//
// Created by luocf on 2019-11-13.
//

#ifndef ELASTOS_SDK_CONTACT_MICROSERVICE_HPP
#define ELASTOS_SDK_CONTACT_MICROSERVICE_HPP
#include <string>
#include <sstream>
#include <ElephantContact.hpp>

namespace elastos {
    static const char *MicroService_TAG = "_MicroService_";
    class MicroService  :public std::enable_shared_from_this<MicroService> {
    public:
        MicroService(){};
        ~MicroService(){};
        virtual void Create(const std::string &pub_key, const std::string &dir) = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void HandleMessage(const std::string &humanCode, const std::string &msg) = 0;
    }; // class MicroService
}
#endif //ELASTOS_SDK_CONTACT_MICROSERVICE_HPP
