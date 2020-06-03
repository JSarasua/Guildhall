#include "Engine/Time/Timer.hpp"
#include "Engine/Time/Clock.hpp"

void Timer::SetSeconds( Clock* clock, double durationSeconds )
{
	m_clock = clock;
	m_startSeconds = m_clock->GetTotalElapsedSeconds();
	m_durationSeconds = durationSeconds;
}

void Timer::SetSeconds( double durationSeconds )
{
	if( nullptr == m_clock )
	{
		m_clock = Clock::GetMaster();
	}

	m_startSeconds = m_clock->GetTotalElapsedSeconds();
	m_durationSeconds = durationSeconds;
	
}

void Timer::Reset()
{
	m_startSeconds = m_clock->GetTotalElapsedSeconds();
	m_isStopped = false;
}

void Timer::Stop()
{
	m_isStopped = true;
}

double Timer::GetElapsedSeconds() const
{
	double currentTime = m_clock->GetTotalElapsedSeconds();

	return currentTime - m_startSeconds;
}

double Timer::GetSecondsRemaining() const
{
	double currentTime = m_clock->GetTotalElapsedSeconds();

	double secondsRemaining = m_durationSeconds - (currentTime - m_startSeconds);
	return secondsRemaining;
}

bool Timer::HasElapsed() const
{
	if( !IsRunning() )
	{
		return false;
	}

	double currentTime = m_clock->GetTotalElapsedSeconds();

	double secondsRemaining = m_durationSeconds - (currentTime - m_startSeconds);
	if( secondsRemaining <= 0.0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Timer::CheckAndDecrement()
{
	if( !IsRunning() )
	{
		return false;
	}

	double currentTime = m_clock->GetTotalElapsedSeconds();

	double secondsRemaining = m_durationSeconds - (currentTime - m_startSeconds);
	if( secondsRemaining < 0.0 )
	{
		m_startSeconds += m_durationSeconds;
		return true;
	}
	else
	{
		return false;
	}
}

int Timer::CheckAndDecrementAll()
{
	if( !IsRunning() )
	{
		return 0;
	}


	double currentTime = m_clock->GetTotalElapsedSeconds();
	int elapsedDurationCounter = 0;

	double secondsRemaining = m_durationSeconds - (currentTime - m_startSeconds);
	while( secondsRemaining < 0.0 )
	{
		m_startSeconds += m_durationSeconds;
		elapsedDurationCounter++;

		secondsRemaining = m_durationSeconds - (currentTime - m_startSeconds);
	}

	return elapsedDurationCounter;
}

bool Timer::IsRunning() const
{
	if( m_isStopped == false )
	{
		return true;
	}
	else
	{
		return false;
	}
}
