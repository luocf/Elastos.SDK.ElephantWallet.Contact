//
//  MessageChannelStrategy.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <MessageChannelStrategy.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
MessageChannelStrategy::MessageChannelStrategy(uint32_t chType,
                                               std::shared_ptr<ChannelListener> chListener,
                                               std::shared_ptr<ChannelDataListener> dataListener)
    : mChannelType(chType)
    , mChannelListener(chListener)
    , mChannelDataListener(dataListener)
{
}

MessageChannelStrategy::~MessageChannelStrategy()
{
}

//MessageChannelStrategy::FileInfo::FileInfo(const std::string& name,
//                                           uint64_t size,
//                                           const std::string& md5)
//    : mName(name)
//    , mSize(size)
//    , mMd5(md5)
//{
//}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
