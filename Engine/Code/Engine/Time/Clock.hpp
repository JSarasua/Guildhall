#pragma once
#include <vector>

class Clock
{
public:
	Clock();
	Clock( Clock* parent );
	~Clock();


	void Update( double deltaSeconds );
	void Reset();

	// Controls
	void Pause();
	void Resume();
	void SetScale( double timeScale );

	//accessors
	double GetTotalElapsedSeconds() const;
	double GetLastDeltaSeconds() const;

	double GetScale() const;
	bool IsPaused() const;

	void SetFrameLimits( double minFrameTime, double maxFrameTime );

	//Used more internally
	void SetParent( Clock* clock );
	void AddChild( Clock* clock );

public:
	static void SystemStartup(); //create/reset master clock
	static void SystemShutdown();
	static void BeginFrame(); //advance master clock

	static Clock* GetMaster();

private:
	std::vector<Clock*> m_childClocks;

	double m_currentTimeSeconds = 0;
	double m_deltaTimeSeconds = 0;

	bool m_isPaused = false;
	double m_timeScale = 1;

	double m_minFrameTime = 0;
	double m_maxFrameTime = 99999999;
};