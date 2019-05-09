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
        virtual std::string onRequestMnemonic() = 0;
    protected:
        explicit SecurityListener() = default;
        virtual ~SecurityListener() = default;
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit SecurityManager();
    virtual ~SecurityManager();

    void setSecurityListener(std::shared_ptr<SecurityListener> listener);

    int generateMnemonic(std::string& value) const;

    int getPublicKey(std::string& value);
    int getElaAddress(std::string& value);
    int getDid(std::string& value);

    int encryptData(const std::string& key, const std::vector<int8_t>& src, std::vector<int8_t>& dest);
    int encryptData(const std::vector<int8_t>& src, std::vector<int8_t>& dest);
    int decryptData(const std::vector<int8_t>& src, std::vector<int8_t>& dest);

    void clear();

private:
    /*** type define ***/

    /*** static function and variable ***/
    constexpr static const char* Language = "english";
    constexpr static const char* Words = "";

    /*** class function and variable ***/
    int getSeed(std::vector<uint8_t>& value);

    std::shared_ptr<SecurityListener> mSecurityListener;

}; // class SecurityManager

} // namespace elastos

#endif /* _ELASTOS_SECURITY_MANAGER_HPP_ */
