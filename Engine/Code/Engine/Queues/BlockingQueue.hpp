#pragma once
#include <iostream>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>


template<typename T>
class BlockingQueue : protected std::queue<T>
{
public:
	using value_type = typename T;

protected:
	//typedef std::queue<T> base;
	using base = typename std::queue<T>;
public:
	BlockingQueue(): base() {}
	BlockingQueue( BlockingQueue const&& ) = delete; //Move constructor. If what I'm copying is temporary, then we move it.
	BlockingQueue( BlockingQueue const& ) = delete;

	BlockingQueue& operator=( BlockingQueue const&& ) = delete;
	BlockingQueue& operator=( BlockingQueue const& ) = delete;

	~BlockingQueue() = default;

	void push( value_type const& value );
	value_type pop();
	void Quit(){ m_condition.notify_all(); }

protected:

private:
	std::mutex m_lock;
	std::condition_variable m_condition;

};

template<typename T>
void BlockingQueue<T>::push( value_type const& value )
{
	std::lock_guard<std::mutex> guard( m_lock ); //takes the place of doing m_lock.lock(); and m_lock.unlock();
	base::push( value );
	m_condition.notify_all();
	//lock_guard gives up lock when going out of scope
}

template<typename T>
typename BlockingQueue<T>::value_type BlockingQueue<T>::pop()
{
	value_type value = value_type();

	std::unique_lock<std::mutex> uniqueLock( m_lock ); //works like lock_guard
	if( base::empty() )
	{
		m_condition.wait( uniqueLock );
		//m_condition.wait_until( uniqueLock, )
	}

	if( !base::empty() )
	{
		value = base::front();
		base::pop();
	}


	return value;
}