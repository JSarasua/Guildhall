#pragma once

class Clock;

class Timer
{
public:
	void SetSeconds( Clock* clock, double durationSeconds );
	void SetSeconds( double durationDeconds );

	void Reset();							// reset timer but it keeps running
	void Stop();							// stops timer

	double GetElapsedSeconds() const;		//return the amount of accrued time
	double GetSecondsRemaining() const;		//return amount of time until HasElapsed() will return true

	bool HasElapsed() const;				//timer has elapsed the timer
	bool CheckAndDecrement();				//if has elapsed, reduce time by duration and return true, else return false
	int CheckAndDecrementAll();				//returns the number of invervals of elapsed time and removes them

	bool IsRunning() const;					//timer is incrementing



public:
	Clock* m_clock;

	double m_startSeconds = 0.0;
	double m_durationSeconds = -1.0;		//negative means stopped

	//double m_runningTimer = 0.0;

	bool m_isStopped = false;


};