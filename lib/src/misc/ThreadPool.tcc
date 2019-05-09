//
//  ThreadPool.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include "ThreadPool.hpp"

#include "Log.hpp"

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
template<class F, class... Args>
ThreadPool<F, Args...>::ThreadPool(const std::string& threadName, size_t threadCnt)
    : mThreadName(threadName)
    , mThreadPool(threadCnt)
    , mMutex()
    , mCondition()
    , mTaskQueue()
    , mQuit(false)
{
    Log::D(Log::TAG, "%s name:%s count:%d", __PRETTY_FUNCTION__, threadName.c_str(), threadCnt);

	for(size_t idx = 0; idx < mThreadPool.size(); idx++) {
		mThreadPool[idx] = std::thread(&ThreadPool<F, Args...>::processTaskQueue, this);
	}
}

template<class F, class... Args>
ThreadPool<F, Args...>::~ThreadPool()
{
    Log::D(Log::TAG, "%s", __PRETTY_FUNCTION__);

	std::unique_lock<std::mutex> lock(mMutex);
	mQuit = true;
    auto empty = std::queue<Task>();
    std::swap(mTaskQueue, empty); // mTaskQueue.clear();
	lock.unlock();
	mCondition.notify_all();

	// Wait for threads to finish before we exit
	for(size_t idx = 0; idx < mThreadPool.size(); idx++) {
		if(mThreadPool[idx].joinable()) {
            Log::D(Log::TAG, "%s Joining thread %d until completion...", __PRETTY_FUNCTION__, idx);
			mThreadPool[idx].join();
		}
	}
    mThreadPool.clear();
}

template<class F, class... Args>
void ThreadPool<F, Args...>::post(const Task& task)
{
	std::unique_lock<std::mutex> lock(mMutex);
	mTaskQueue.push(task);

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	mCondition.notify_all();
}

template<class F, class... Args>
void ThreadPool<F, Args...>::post(Task&& task)
{
	std::unique_lock<std::mutex> lock(mMutex);
	mTaskQueue.push(std::move(task));

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	mCondition.notify_all();
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
template<class F, class... Args>
void ThreadPool<F, Args...>::processTaskQueue(void)
{
	std::unique_lock<std::mutex> lock(mMutex);

	do {
		//Wait until we have data or a quit signal
		mCondition.wait(lock, [this]{
			return (mTaskQueue.size() || mQuit);
		});

		//after wait, we own the lock
		if(!mQuit && mTaskQueue.size())
		{
			auto task = std::move(mTaskQueue.front());
			mTaskQueue.pop();

			//unlock now that we're done messing with the queue
			lock.unlock();

			task();

			lock.lock();
		}
	} while (!mQuit);
}

} // namespace elastos
