#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

//------------------------------------------------------------------------------------------------
Timer::Timer(double period, const Clock* clock)
	: m_period(period)
{
	if (clock != nullptr) {
		m_clock = clock;
	}
	else {
		m_clock = &Clock::GetSystemClock();
	}
}

//------------------------------------------------------------------------------------------------
void Timer::Start()
{
	if (m_clock == nullptr) {
		m_clock = &Clock::GetSystemClock();
	}

	m_startTime = m_clock->GetTotalSeconds();
}

//------------------------------------------------------------------------------------------------
void Timer::Stop()
{
	m_startTime = -1.0f;
}

//------------------------------------------------------------------------------------------------
double Timer::GetElapsedTime() const
{
	if (IsStopped() || m_clock == nullptr) {
		return 0.0;
	}

	return m_clock->GetTotalSeconds() - m_startTime;
}

//------------------------------------------------------------------------------------------------
double Timer::GetElapsedFraction() const
{
	if (m_period <= 0.0) {
		return 0.0;
	}

	return GetElapsedTime() / m_period;
}

//------------------------------------------------------------------------------------------------
bool Timer::IsStopped() const
{
	return m_startTime < 0.0;
}

//------------------------------------------------------------------------------------------------
bool Timer::HasPeriodElapsed() const
{
	if (IsStopped()) {
		return false;
	}

	return GetElapsedTime() > m_period;
}

//------------------------------------------------------------------------------------------------
bool Timer::DecrementPeriodIfElapsed()
{
	if (!HasPeriodElapsed()) {
		return false;
	}

	m_startTime += m_period;
	return true;
}

