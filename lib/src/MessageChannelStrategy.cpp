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
MessageChannelStrategy::MessageChannelStrategy()
    : mChannelListener()
{
}

MessageChannelStrategy::~MessageChannelStrategy()
{
}

void MessageChannelStrategy::setChannelListener(std::shared_ptr<ChannelListener> listener)
{
    mChannelListener = listener;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
