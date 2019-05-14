/**
 * @file	SecurityManager.hpp
 * @brief	SecurityManager
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SECURITY_MANAGER_HPP_
#define _ELASTOS_SECURITY_MANAGER_HPP_

#include <string>
#include <vector>

#include "ErrCode.hpp"

namespace elastos {

class SecurityManager final {
public:
    /*** type define ***/
    class SecurityListener {
    public:
        virtual std::string onRequestPublicKey() = 0;

        virtual std::vector<int8_t> onEncryptData(const std::string& pubKey, const std::vector<int8_t>& src) = 0;
        virtual std::vector<int8_t> onDecryptData(const std::vector<int8_t>& src) = 0;
    protected:
        explicit SecurityListener() = default;
        virtual ~SecurityListener() = default;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit SecurityManager();
    virtual ~SecurityManager();

    void setSecurityListener(std::shared_ptr<SecurityListener> listener);

    int getPublicKey(std::string& pubKey);
    int getElaAddress(std::string& elaAddr);
    int getDid(std::string& did);

    int encryptData(const std::string& key, const std::vector<int8_t>& src, std::vector<int8_t>& dest);
    int decryptData(const std::vector<int8_t>& src, std::vector<int8_t>& dest);

    void clear();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::shared_ptr<SecurityListener> mSecurityListener;

}; // class SecurityManager

} // namespace elastos

#endif /* _ELASTOS_SECURITY_MANAGER_HPP_ */
