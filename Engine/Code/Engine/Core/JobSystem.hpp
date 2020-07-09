#pragma once
#include <deque>
#include <vector>
#include <thread>
#include <mutex>

class WorkerThread
{
	friend class JobSystem;
public:
	WorkerThread();
	~WorkerThread();
	
	void WorkerMain();


protected:
	std::thread* m_workerThread = nullptr;
};



class Job
{
public:
	Job();
	virtual ~Job() {}

	virtual void Execute() = 0;
	virtual void CallBackFunction() = 0;


protected:
	int m_jobID = 0;
};



class JobSystem
{
	friend class WorkerThread;
public:
	JobSystem();
	~JobSystem();

	void AddWorkerThread();
	void AddWorkerThreads( int numberOfThreadsToAdd );
	void ClaimAndDeleteCompletedJobs();
	void PostJob( Job* job );

protected:
	std::deque<Job*> m_jobsQueued;
	std::deque<Job*> m_jobsCompleted;
	std::mutex m_jobsQueuedLock;
	std::mutex m_jobsCompletedLock;

	std::vector<WorkerThread*> m_workerThreads;
};



