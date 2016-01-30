#ifndef _H_TIMING
#define _H_TIMING

#include <Windows.h>

class HighResolutionTimer {
public:
	HighResolutionTimer();
	~HighResolutionTimer();

	void Start();
	void Stop();
	void Reset();

	long long AsMicroseconds();
	double AsMilliseconds();
	//float AsSeconds();

private:
	bool is_started;

	long long get_ticks();

	LARGE_INTEGER m_StartingTime;
	LARGE_INTEGER m_EndingTime;
	LARGE_INTEGER m_ClockFrequency;
};



#endif