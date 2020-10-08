#pragma once
#include <iostream>
#include <queue>
#include <condition_variable>
//#include <mutex>
//#include <thread>
#include <atomic>

template <typename T>
class SynchronizedLockFreeQueue : protected std::queue<T>
{
public:
	using value_type = typename T;
protected:
	using base = typename std::queue<T>;

public:
	SynchronizedLockFreeQueue(): base() {}
	SynchronizedLockFreeQueue( SynchronizedLockFreeQueue const&& ) = delete;
	SynchronizedLockFreeQueue( SynchronizedLockFreeQueue const& ) = delete;

	~SynchronizedLockFreeQueue() = default;

	SynchronizedLockFreeQueue& operator=( SynchronizedLockFreeQueue const&& ) = delete;
	SynchronizedLockFreeQueue& operator=( SynchronizedLockFreeQueue const& ) = delete;

	void push( value_type const& valToPush );
	value_type pop();
	int size();

protected:
	void lock();
	void unlock();
private:
	const int _UNLOCKED = 0;
	const int _LOCKED = 1;
	std::atomic<int> m_atomicLock;
};

template <typename T>
int SynchronizedLockFreeQueue<T>::size()
{
	lock();
	int size = (int)base::size();
	unlock();

	return size;
}

template <typename T>
void SynchronizedLockFreeQueue<T>::lock()
{
	int expected = _UNLOCKED;
	while( m_atomicLock.compare_exchange_strong( expected, _LOCKED ) );
}

template <typename T>
void SynchronizedLockFreeQueue<T>::unlock()
{
	int expected = _LOCKED;
	while( m_atomicLock.compare_exchange_strong( expected, _UNLOCKED ) );
}

template<typename T>
void SynchronizedLockFreeQueue<T>::push( value_type const& valToPush )
{
	lock();
	base::push( valToPush );
	unlock();
}


template <typename T>
typename SynchronizedLockFreeQueue<T>::value_type SynchronizedLockFreeQueue<T>::pop()
{
	value_type value;// = value_type( 0 );
	lock();
	if( !base::empty() )
	{
		value = base::front();
		base::pop();
	}
	unlock();
	return value;
}
