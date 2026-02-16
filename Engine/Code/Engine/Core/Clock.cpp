#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"

//------------------------------------------------------------------------------------------------
static bool g_isConstructingSystemClock = false;

//------------------------------------------------------------------------------------------------
Clock::Clock()
{
	if (!g_isConstructingSystemClock) {
		m_parent = &GetSystemClock();
		m_parent->AddChild(this);
	}

	Reset();
}

//------------------------------------------------------------------------------------------------
Clock::Clock(Clock& parent)
	: m_parent(&parent)
{
	m_parent->AddChild(this);
	Reset();
}

//------------------------------------------------------------------------------------------------
Clock::~Clock()
{
	if (m_parent != nullptr) {
		m_parent->RemoveChild(this);
		m_parent = nullptr;
	}

	for (Clock* child : m_children) {
		if (child != nullptr) {
			child->m_parent = nullptr;
		}
	}

	m_children.clear();
}

//------------------------------------------------------------------------------------------------
void Clock::Reset()
{
	m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();
	m_totalSeconds = 0.0;
	m_deltaSeconds = 0.0;
	m_frameCount = 0;
	m_timeScale = 1.0;
	m_isPaused = false;
	m_stepSingleFrame = false;
}

//------------------------------------------------------------------------------------------------
bool Clock::IsPaused() const
{
	return m_isPaused;
}

//------------------------------------------------------------------------------------------------
void Clock::Pause()
{
	m_isPaused = true;
}

//------------------------------------------------------------------------------------------------
void Clock::Unpause()
{
	m_isPaused = false;
	m_stepSingleFrame = false;
}

//------------------------------------------------------------------------------------------------
void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
	if (!m_isPaused) {
		m_stepSingleFrame = false;
	}
}

//------------------------------------------------------------------------------------------------
void Clock::StepSingleFrame()
{
	m_isPaused = true;
	m_stepSingleFrame = true;
}

//------------------------------------------------------------------------------------------------
void Clock::SetTimeScale(double timeScale)
{
	if (timeScale < 0.0) {
		timeScale = 0.0;
	}

	m_timeScale = timeScale;
}

//------------------------------------------------------------------------------------------------
double Clock::GetTimeScale() const
{
	return m_timeScale;
}

//------------------------------------------------------------------------------------------------
double Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}

//------------------------------------------------------------------------------------------------
double Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

//------------------------------------------------------------------------------------------------
double Clock::GetFrameRate() const
{
	if (m_deltaSeconds <= 0.0) {
		return 0.0;
	}

	return 1.0 / m_deltaSeconds;
}

//------------------------------------------------------------------------------------------------
int Clock::GetFrameCount() const
{
	return m_frameCount;
}

//------------------------------------------------------------------------------------------------
Clock& Clock::GetSystemClock()
{
	static Clock* s_systemClock = nullptr;
	if (s_systemClock == nullptr) {
		g_isConstructingSystemClock = true;
		s_systemClock = new Clock();
		g_isConstructingSystemClock = false;
	}

	return *s_systemClock;
}

//------------------------------------------------------------------------------------------------
void Clock::TickSystemClock()
{
	GetSystemClock().Tick();
}

//------------------------------------------------------------------------------------------------
void Clock::Tick()
{
	double currentTimeSeconds = GetCurrentTimeSeconds();
	double deltaTimeSeconds = currentTimeSeconds - m_lastUpdateTimeInSeconds;
	m_lastUpdateTimeInSeconds = currentTimeSeconds;

	if (deltaTimeSeconds < 0.0) {
		deltaTimeSeconds = 0.0;
	}
	if (deltaTimeSeconds > m_maxDeltaSeconds) {
		deltaTimeSeconds = m_maxDeltaSeconds;
	}

	Advance(deltaTimeSeconds);
}

//------------------------------------------------------------------------------------------------
void Clock::Advance(double deltaTimeSeconds)
{
	double scaledDeltaSeconds = deltaTimeSeconds * m_timeScale;

	if (m_isPaused) {
		if (m_stepSingleFrame) {
			m_stepSingleFrame = false;
		}
		else {
			scaledDeltaSeconds = 0.0;
		}
	}

	m_deltaSeconds = scaledDeltaSeconds;
	m_totalSeconds += m_deltaSeconds;
	++m_frameCount;

	for (Clock* child : m_children) {
		if (child != nullptr) {
			child->Advance(m_deltaSeconds);
		}
	}
}

//------------------------------------------------------------------------------------------------
void Clock::AddChild(Clock* childClock)
{
	if (childClock == nullptr) {
		return;
	}

	for (Clock* child : m_children) {
		if (child == childClock) {
			return;
		}
	}

	m_children.push_back(childClock);
}

//------------------------------------------------------------------------------------------------
void Clock::RemoveChild(Clock* childClock)
{
	if (childClock == nullptr) {
		return;
	}

	for (std::vector<Clock*>::iterator it = m_children.begin(); it != m_children.end(); ++it) {
		if (*it == childClock) {
			m_children.erase(it);
			return;
		}
	}
}

