#ifndef _CONTACT_TEST_CMD_HPP_
#define _CONTACT_TEST_CMD_HPP_

#include <Elastos.SDK.Contact.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class ContactTestCmd {
public:
    /*** type define ***/

    /*** static function and variable ***/
    static int Do(std::shared_ptr<elastos::Contact> contact,
                  const std::string& cmdLine,
                  std::string& errMsg);

    /*** class function and variable ***/

protected:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/

private:
    /*** type define ***/
    struct CommandInfo {
        char mCmd;
        std::string mLongCmd;
        std::function<int(std::shared_ptr<elastos::Contact>, const std::vector<std::string>&, std::string&)> mFunc;
        std::string mUsage;
    };

    /*** static function and variable ***/
    static int Help(std::shared_ptr<elastos::Contact> contact,
                    const std::vector<std::string>& args,
                    std::string& errMsg);
    static int PrintInfo(std::shared_ptr<elastos::Contact> contact,
                         const std::vector<std::string>& args,
                         std::string& errMsg);
    static int AddFriend(std::shared_ptr<elastos::Contact> contact,
                         const std::vector<std::string>& args,
                         std::string& errMsg);

    static const std::vector<CommandInfo> gCommandInfoList;

    /*** class function and variable ***/
    explicit ContactTestCmd() = delete;
    virtual ~ContactTestCmd() = delete;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

#endif /* _CONTACT_TEST_CMD_HPP_ */

