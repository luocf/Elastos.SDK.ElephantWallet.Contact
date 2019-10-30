//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactDataListener.hpp>

#include <ContactDataListener.proxy.hpp>
#include "Log.hpp"

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
ContactDataListener* ContactDataListener::sContactDataListenerInstance = nullptr;

/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ContactDataListener::ContactDataListener()
        : mDataListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    sContactDataListenerInstance = this;

    mDataListener = makeDataListener();
}
ContactDataListener::~ContactDataListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
    if(sContactDataListenerInstance == this) {
        sContactDataListenerInstance = nullptr;
    }
}

std::shared_ptr<elastos::MessageManager::DataListener> ContactDataListener::getDataListener()
{
    return mDataListener;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
std::shared_ptr<elastos::MessageManager::DataListener> ContactDataListener::makeDataListener()
{
    class DataListener final : public elastos::MessageManager::DataListener {
    public:
        explicit DataListener() = default;
        virtual ~DataListener() = default;

        virtual void onResult(std::shared_ptr<elastos::HumanInfo> humanInfo,
                              elastos::MessageManager::ChannelType channelType,
                              const std::string& dataId,
                              int errCode) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = humanInfo->getHumanCode(humanCode);
            CHECK_RETVAL(ret);

            sContactDataListenerInstance->onResult(humanCode,
                                                   static_cast<ContactListener::ContactChannel>(channelType),
                                                   dataId, errCode);
        }

        virtual int onReadData(std::shared_ptr<elastos::HumanInfo> humanInfo,
                               elastos::MessageManager::ChannelType channelType,
                               const std::string& dataId, uint64_t offset,
                               std::vector<uint8_t>& data) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = humanInfo->getHumanCode(humanCode);
            CHECK_ERROR(ret);


            auto readData = sContactDataListenerInstance->onReadData(humanCode,
                                                                     static_cast<ContactListener::ContactChannel>(channelType),
                                                                     dataId, offset);
            if(readData.get() == nullptr) {
                return elastos::ErrCode::ChannelFailedReadData;
            }

            data.clear();
            data.insert(data.begin(), readData->data(), readData->data() + readData->size());
            return data.size();
        }

        virtual int onWriteData(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                elastos::MessageManager::ChannelType channelType,
                                const std::string& dataId, uint64_t offset,
                                const std::vector<uint8_t>& data) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = humanInfo->getHumanCode(humanCode);
            CHECK_ERROR(ret);

            std::span<uint8_t> writeData(reinterpret_cast<uint8_t*>(const_cast<uint8_t*>(data.data())), data.size());
            ret = sContactDataListenerInstance->onWriteData(humanCode,
                                                            static_cast<ContactListener::ContactChannel>(channelType),
                                                            dataId, offset, &writeData);
            return ret;
        }
    };

    return std::make_shared<DataListener>();
}

void ContactDataListener::onResult(const std::string& humanCode,
                                   ContactListener::ContactChannel channelType,
                                   const std::string& dataId,
                                   int errCode)
{
    int64_t platformHandle = getPlatformHandle();
    crosspl::proxy::ContactDataListener::onResult(platformHandle,
                                                  humanCode.c_str(), static_cast<int>(channelType),
                                                  dataId.c_str(), errCode);
}

std::shared_ptr<std::span<uint8_t>> ContactDataListener::onReadData(const std::string& humanCode,
                                                                    ContactListener::ContactChannel channelType,
                                                                    const std::string& dataId,
                                                                    uint64_t offset)
{
    int64_t platformHandle = getPlatformHandle();
    auto ret = crosspl::proxy::ContactDataListener::onReadData(platformHandle,
                                                           humanCode.c_str(), static_cast<int>(channelType),
                                                           dataId.c_str(), offset);

    return ret;
}

int ContactDataListener::onWriteData(const std::string& humanCode,
                                     ContactListener::ContactChannel channelType,
                                     const std::string& dataId,
                                     uint64_t offset,
                                     const std::span<uint8_t>* data)
{
    int64_t platformHandle = getPlatformHandle();
    auto ret = crosspl::proxy::ContactDataListener::onWriteData(platformHandle,
                                                           humanCode.c_str(), static_cast<int>(channelType),
                                                           dataId.c_str(), offset, data);

    return ret;
}
