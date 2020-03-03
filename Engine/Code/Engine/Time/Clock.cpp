#include "Engine/Time/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"

static Clock* s_theMasterClock = nullptr;

Clock::Clock()
{

}

Clock::Clock( Clock* parent )
{
	SetParent( parent );
}

Clock::~Clock()
{
	for( size_t childClockIndex = 0; childClockIndex < m_childClocks.size(); childClockIndex++ )
	{
		delete m_childClocks[childClockIndex];
		m_childClocks[childClockIndex] = nullptr;
	}
}

void Clock::Update( double deltaSeconds )
{
	if( !m_isPaused )
	{
		double tempDeltaSeconds = ClampDouble( deltaSeconds, m_minFrameTime, m_maxFrameTime );
		m_deltaTimeSeconds = tempDeltaSeconds * m_timeScale;
		m_currentTimeSeconds += m_deltaTimeSeconds;

	}

	for( size_t childClockIndex = 0; childClockIndex < m_childClocks.size(); childClockIndex++ )
	{
		if( nullptr == m_childClocks[childClockIndex] )
		{
			continue;
		}

		m_childClocks[childClockIndex]->Update(m_deltaTimeSeconds);
	}
}

void Clock::Reset()
{
	m_currentTimeSeconds = 0;
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::Resume()
{
	m_isPaused = false;
}

void Clock::SetScale( double timeScale )
{
	m_timeScale = timeScale;
}

double Clock::GetTotalElapsedSeconds() const
{
	return m_currentTimeSeconds;
}

double Clock::GetLastDeltaSeconds() const
{
	return m_deltaTimeSeconds;
}

double Clock::GetScale() const
{
	return m_timeScale;
}

bool Clock::IsPaused() const
{
	return m_isPaused;
}

void Clock::SetFrameLimits( double minFrameTime, double maxFrameTime )
{
	m_minFrameTime = minFrameTime;
	m_maxFrameTime = maxFrameTime;
}

void Clock::SetParent( Clock* clock )
{
	if( nullptr != clock )
	{
		clock->AddChild( this );
	}
}

void Clock::AddChild( Clock* clock )
{
	m_childClocks.push_back( clock );
}

void Clock::SystemStartup()
{
	if( s_theMasterClock == nullptr )
	{
		s_theMasterClock = new Clock( nullptr );
	}
	s_theMasterClock->Reset();
}

void Clock::SystemShutdown()
{

}

void Clock::BeginFrame()
{
	static double timePreviousFrame = GetCurrentTimeSeconds();
	double timeThisFrame = GetCurrentTimeSeconds();

	double deltaTimeSeconds = timeThisFrame - timePreviousFrame;

	timePreviousFrame = timeThisFrame;

	s_theMasterClock->Update( deltaTimeSeconds );
	
}

Clock* Clock::GetMaster()
{
	return s_theMasterClock;
}
