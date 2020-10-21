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

	while( m_jobsRunning.size() > 0 )
	{
		Job* currentJob = m_jobsRunning.front();
		delete currentJob;
		m_jobsRunning.pop_front();
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
	m_workerThreads.push_back( new WorkerThread( this ) );
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

void JobSystem::PostPriorityJob( Job* job )
{
	m_priorityJobsQueuedLock.lock();
	m_priorityJobsQueued.push_back( job );
	m_priorityJobsQueuedLock.unlock();
}

int JobSystem::GetNumberOfJobsQueued()
{
	int numberOfJobsQueued = 0;
	m_jobsQueuedLock.lock();
	numberOfJobsQueued = (int)m_jobsQueued.size();
	m_jobsQueuedLock.unlock();

	return numberOfJobsQueued;
}

WorkerThread::WorkerThread( JobSystem* owningJobSystem ) : m_owningJobSystem( owningJobSystem )
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
		m_owningJobSystem->m_priorityJobsQueuedLock.lock();
		if( !m_owningJobSystem->m_priorityJobsQueued.empty() )
		{
			Job* jobToWork = m_owningJobSystem->m_priorityJobsQueued.front();
			m_owningJobSystem->m_priorityJobsQueued.pop_front();
			m_owningJobSystem->m_priorityJobsQueuedLock.unlock();

			m_owningJobSystem->m_jobsRunningLock.lock();
			std::deque<Job*>& jobsRunningPreRun = m_owningJobSystem->m_jobsRunning;
			bool didAddJob = false;
			for( size_t jobsRunningIndex = 0; jobsRunningIndex < jobsRunningPreRun.size(); jobsRunningIndex++ )
			{
				Job* currentJob = jobsRunningPreRun[jobsRunningIndex];
				if( !currentJob )
				{
					jobsRunningPreRun[jobsRunningIndex] = jobToWork;
					didAddJob = true;
					break;
				}
			}
			if( !didAddJob )
			{
				jobsRunningPreRun.push_back( jobToWork );
			}
			m_owningJobSystem->m_jobsRunningLock.unlock();

			jobToWork->Execute();

			m_owningJobSystem->m_jobsRunningLock.lock();
			std::deque<Job*>& jobsRunning = m_owningJobSystem->m_jobsRunning;
			for( size_t jobsRunningIndex = 0; jobsRunningIndex < jobsRunning.size(); jobsRunningIndex++ )
			{
				Job* currentJob = jobsRunning[jobsRunningIndex];
				if( jobToWork == currentJob )
				{
					jobsRunning[jobsRunningIndex] = nullptr;
				}
			}
			m_owningJobSystem->m_jobsRunningLock.unlock();

			m_owningJobSystem->m_jobsCompletedLock.lock();
			m_owningJobSystem->m_jobsCompleted.push_back( jobToWork );
			m_owningJobSystem->m_jobsCompletedLock.unlock();
		}
		else
		{
			m_owningJobSystem->m_priorityJobsQueuedLock.unlock();
		}

		m_owningJobSystem->m_jobsQueuedLock.lock();
		if( !m_owningJobSystem->m_jobsQueued.empty() )
		{
			Job* jobToWork = m_owningJobSystem->m_jobsQueued.front();
			m_owningJobSystem->m_jobsQueued.pop_front();
			m_owningJobSystem->m_jobsQueuedLock.unlock();

			m_owningJobSystem->m_jobsRunningLock.lock();
			std::deque<Job*>& jobsRunningPreRun = m_owningJobSystem->m_jobsRunning;
			bool didAddJob = false;
			for( size_t jobsRunningIndex = 0; jobsRunningIndex < jobsRunningPreRun.size(); jobsRunningIndex++ )
			{
				Job* currentJob = jobsRunningPreRun[jobsRunningIndex];
				if( !currentJob )
				{
					jobsRunningPreRun[jobsRunningIndex] = jobToWork;
					didAddJob = true;
					break;
				}
			}
			if( !didAddJob )
			{
				jobsRunningPreRun.push_back( jobToWork );
			}
			m_owningJobSystem->m_jobsRunningLock.unlock();

			jobToWork->Execute();

			m_owningJobSystem->m_jobsRunningLock.lock();
			std::deque<Job*>& jobsRunning = m_owningJobSystem->m_jobsRunning;
			for( size_t jobsRunningIndex = 0; jobsRunningIndex < jobsRunning.size(); jobsRunningIndex++ )
			{
				Job* currentJob = jobsRunning[jobsRunningIndex];
				if( jobToWork == currentJob )
				{
					jobsRunning[jobsRunningIndex] = nullptr;
				}
			}
			m_owningJobSystem->m_jobsRunningLock.unlock();

			m_owningJobSystem->m_jobsCompletedLock.lock();
			m_owningJobSystem->m_jobsCompleted.push_back( jobToWork );
			m_owningJobSystem->m_jobsCompletedLock.unlock();
		}
		else
		{
			m_owningJobSystem->m_jobsQueuedLock.unlock();

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
