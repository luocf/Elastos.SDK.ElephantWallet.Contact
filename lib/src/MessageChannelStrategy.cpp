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
                                               std::shared_ptr<ChannelListener> listener)
    : mChannelType(chType)
    , mChannelListener(listener)
{
}

MessageChannelStrategy::~MessageChannelStrategy()
{
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
