#include "timing.h"

HighResolutionTimer::HighResolutionTimer() : is_started(0)
{	
	QueryPerformanceFrequency(&m_ClockFrequency);
}

HighResolutionTimer::~HighResolutionTimer() { }

void HighResolutionTimer::Start()
{
	QueryPerformanceCounter(&m_StartingTime);
	is_started = true;
}

void HighResolutionTimer::Stop()
{
	m_StartingTime.QuadPart = 0;
	is_started = false;	
}

void HighResolutionTimer::Reset()
{
	QueryPerformanceCounter(&m_StartingTime);
}

long long HighResolutionTimer::get_ticks()
{	
	if (is_started == true) {
		QueryPerformanceCounter(&m_EndingTime);

		return m_EndingTime.QuadPart - m_StartingTime.QuadPart;
	}
	else {
		return -1;
	}
}

long long HighResolutionTimer::AsMicroseconds()
{
	return (get_ticks() * 1000000 / m_ClockFrequency.QuadPart);
}

double HighResolutionTimer::AsMilliseconds()
{
	return (double)(get_ticks() * 1000) / m_ClockFrequency.QuadPart;
}
