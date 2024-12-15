#pragma once

#include <list>
#include <mutex>

template< typename T >
class threadsafe_queue
{
public:
    using value_type = T;

	threadsafe_queue() : mutex_(), list_()
	{

	}

	~threadsafe_queue()
	{

	}

	size_t size()
	{
		return list_.size();
	}

	bool empty()
	{
		return list_.empty();
	}

	void push( T t )
	{
		mutex_.lock();
		list_.push_back( t );
		mutex_.unlock();
	}

	void pop()
	{
		mutex_.lock();
		if( !list_.empty() )
		{
			list_.pop_front();
		}
		mutex_.unlock();
	}

	bool front( T& result )
	{
		bool ret = false;
		mutex_.lock();
		if( !list_.empty() )
		{
			result = list_.front();
			ret = true;
		}
		mutex_.unlock();
		return ret;
	}

	// Saves time by poping in the same lock that we get front. Only one lock.
	bool front_and_pop( T& result )
	{
		bool ret = false;
		mutex_.lock();
		if( !list_.empty() )
		{
			result = list_.front();
			list_.pop_front();
			ret = true;
		}
		mutex_.unlock();
		return ret;
	}

	void clear()
	{
		mutex_.lock();
		list_.clear();
		mutex_.unlock();
	}

private:
	std::mutex mutex_;
	std::list< T > list_;
};