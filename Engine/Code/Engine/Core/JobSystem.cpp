#include "Engine/Core/JobSystem.hpp"
#include <chrono>
#include <atomic>
JobSystem* g_theJobSystem = nullptr;


std::atomic<bool> g_isQuitting = false;

JobSystem::JobSystem()
{

}

JobSystem::~JobSystem()
{

}

void JobSystem::Shutdown()
{
	g_isQuitting = true;

	for( size_t workerIndex = 0; workerIndex < m_workerThreads.size(); workerIndex++ )
	{
		m_workerThreads[workerIndex]->m_workerThread->join();
	}

	for( size_t workerIndex = 0; workerIndex < m_workerThreads.size(); workerIndex++ )
	{
		delete m_workerThreads[workerIndex];
	}

	while( m_jobsQueued.size() > 0 )
	{
		Job* currentJob = m_jobsQueued.front();
		delete currentJob;
		m_jobsQueued.pop_front();
	}

	while( m_jobsCompleted.size() > 0 )
	{
		Job* currentJob = m_jobsCompleted.front();
		delete currentJob;
		m_jobsCompleted.pop_front();
	}
}

void JobSystem::AddWorkerThread()
{
	m_workerThreads.push_back( new WorkerThread() );
}

void JobSystem::AddWorkerThreads( int numberOfThreadsToAdd )
{
	for( int threadIndex = 0; threadIndex < numberOfThreadsToAdd; threadIndex++ )
	{
		AddWorkerThread();
	}
}

void JobSystem::ClaimAndDeleteCompletedJobs()
{
	std::deque<Job*> completedJobs;

	m_jobsCompletedLock.lock();
	m_jobsCompleted.swap( completedJobs );
	m_jobsCompletedLock.unlock();

	while( completedJobs.size() > 0 )
	{
		Job* currentJob = completedJobs.front();
		completedJobs.pop_front();

		currentJob->CallBackFunction();
	}
}

void JobSystem::PostJob( Job* job )
{
	m_jobsQueuedLock.lock();
	m_jobsQueued.push_back( job );
	m_jobsQueuedLock.unlock();
}

WorkerThread::WorkerThread()
{
	m_workerThread = new std::thread( &WorkerThread::WorkerMain, this );
}

WorkerThread::~WorkerThread()
{
	delete m_workerThread;
}

void WorkerThread::WorkerMain()
{
	while( !g_isQuitting )
	{
		g_theJobSystem->m_jobsQueuedLock.lock();
		if( !g_theJobSystem->m_jobsQueued.empty() )
		{
			Job* jobToWork = g_theJobSystem->m_jobsQueued.front();
			g_theJobSystem->m_jobsQueued.pop_front();
			g_theJobSystem->m_jobsQueuedLock.unlock();

			jobToWork->Execute();

			g_theJobSystem->m_jobsCompletedLock.lock();
			g_theJobSystem->m_jobsCompleted.push_back( jobToWork );
			g_theJobSystem->m_jobsCompletedLock.unlock();
		}
		else
		{
			g_theJobSystem->m_jobsQueuedLock.unlock();

			std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
		}
	}
}

Job::Job()
{
	static int jobID = 0;
	m_jobID = jobID;
	jobID++;
}
