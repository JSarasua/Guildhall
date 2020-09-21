// BlockingQueue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
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

protected:
    void lock();
    void unlock();
private:
    const int _UNLOCKED = 0;
    const int _LOCKED = 1;
    std::atomic<int> m_atomicLock;
};

template <typename T>
void SynchronizedLockFreeQueue<T>::lock()
{
     int expected = _UNLOCKED;
    while (m_atomicLock.compare_exchange_strong( expected, _LOCKED ) );
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
    value_type value = value_type( 0 );
    lock();
    if( !base::empty() )
    {
        value = base::front();
        base::pop();
    }
    unlock();
    return value;
}


template<typename T>
class BlockingQueue : protected std::queue<T>
{
public:
    using value_type = typename T;
    
protected:
    //typedef std::queue<T> base;
    using base = typename std::queue<T>;
public:
    BlockingQueue() : base() {}
    BlockingQueue( BlockingQueue const&& ) = delete; //Move constructor. If what I'm copying is temporary, then we move it.
    BlockingQueue( BlockingQueue const& ) = delete;

    BlockingQueue& operator=( BlockingQueue const&& ) = delete;
    BlockingQueue& operator=( BlockingQueue const& ) = delete;

    ~BlockingQueue() = default;

    void push( value_type const& value );
    value_type pop();


protected:

private:
    std::mutex m_lock;
    std::condition_variable m_condition;

};

template<typename T>
void BlockingQueue<T>::push( value_type const& value )
{
    std::lock_guard<std::mutex> guard(m_lock); //takes the place of doing m_lock.lock(); and m_lock.unlock();
    base::push( value );
    m_condition.notify_all();
    //lock_guard gives up lock when going out of scope
}

template<typename T>
typename BlockingQueue<T>::value_type BlockingQueue<T>::pop()
{
    value_type value = value_type();
    
    std::unique_lock<std::mutex> uniqueLock(m_lock); //works like lock_guard
    if( base::empty() )
    {
        m_condition.wait( uniqueLock );
    }

    value = base::front();
    base::pop();

    return value;
}

template<typename QueueT>
void readQueue( QueueT& queue )
{
    int val = QueueT::value_type(0);
    do 
    {
        val = queue.pop();
        std::cout << std::this_thread::get_id() << " Popped value: " << val << std::endl;
    } 
    while ( val != QueueT::value_type(-1) );
    std::cout << std::this_thread::get_id() << " Exiting" << std::endl;
}

int main()
{
    BlockingQueue<int> blockingQueue;

    std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;

    std::thread blockingReader( readQueue<BlockingQueue<int>>, std::ref( blockingQueue ) );

    blockingQueue.push(1);
    blockingQueue.push(23);
    blockingQueue.push(36);

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

	blockingQueue.push( 7 );
	blockingQueue.push( 98 );
	blockingQueue.push( 63 );
	blockingQueue.push( 8 );

    std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
    blockingQueue.push( -1 );

    blockingReader.join();

    std::atomic<int> a;
    std::cout << "On this implementation int is always lock free: " << a.is_lock_free();

    SynchronizedLockFreeQueue<int> synchronizedQueue;

    std::thread nonBlockingReader( readQueue<SynchronizedLockFreeQueue<int>>, std::ref( synchronizedQueue ) );

    for( int idx = 1; idx < 20; ++idx )
    {
        std::cout << std::this_thread::get_id() << " Pushing Value: " << idx << std::endl;
        synchronizedQueue.push( idx );
        if( idx % 3 == 0 )
        {
            std::this_thread::sleep_for( std::chrono::microseconds( 50 ) );
        }
    }
    synchronizedQueue.push( -1 );

    nonBlockingReader.join();    
}

