#include "Timer.h"
#include "windows.h"


/*
	//static __int64 qpcStart = 0;
-	//static double freq = 0.0;
-	//static bool firstTime = true;
-	//if(firstTime)
-	//{
-	//	__int64 qpcFreq;
-	//	QueryPerformanceFrequency((LARGE_INTEGER *)&qpcFreq);
-	//	freq = 1.0/(double)qpcFreq;
-	//	QueryPerformanceCounter((LARGE_INTEGER *)&qpcStart);
-	//	firstTime = false;
-	//}
-
-	//__int64 qpcNow = 0;
-	//QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
-	//double timeInSec = (qpcNow - qpcStart) * freq;
-	//double timeInMS = timeInSec * 1000.0;
-	//float scale = static_cast<float>((2.0 * D3DX_PI)/2000.0);
-	//float rot = static_cast<float>(timeInMS * scale);
*/
namespace Helix {

// ****************************************************************************
// ****************************************************************************
Timer::Timer()
: m_running(false)
{
	__int64 qpcFreq;
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_freq);
}

Timer::~Timer()
{}

// ****************************************************************************
// ****************************************************************************
void Timer::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&m_start);
	m_lastLapStart = m_start;
	m_running = true;
}

// ****************************************************************************
// ****************************************************************************
void Timer::Stop()
{
	if(m_running)
	{
		__int64 qpcNow = 0;
		QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
		m_elapsed = (float)((double)(qpcNow - m_start) / (double)m_freq);
	}

	m_running = false;
}

// ****************************************************************************
// ****************************************************************************
float Timer::CurrentElapsedSeconds()
{
	__int64 qpcNow = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
	double elapsed = (double)(qpcNow - m_start) / (double)m_freq;
	return static_cast<float>(elapsed);
}

// ****************************************************************************
// ****************************************************************************
float Timer::CurrentElapsedMilliseconds()
{
	__int64 qpcNow = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
	double elapsed = ((double)(qpcNow - m_start) * 1000.0) / (double)m_freq;
	return static_cast<float>(elapsed);
}

// ****************************************************************************
// ****************************************************************************
float Timer::ElapsedSeconds()
{
	if(!m_running)
	{
		return static_cast<float>(m_elapsed);
	}

	return CurrentElapsedSeconds();
}

// ****************************************************************************
// ****************************************************************************
float Timer::ElapsedMilliseconds()
{
	if(!m_running)
	{
		return static_cast<float>(m_elapsed * 1000.0);
	}

	return CurrentElapsedMilliseconds();
}

// ****************************************************************************
// ****************************************************************************
void Timer::LapMark()
{
	__int64 qpcNow = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
	m_lastLapStart = qpcNow;
}

// ****************************************************************************
// ****************************************************************************
float Timer::LapSeconds()
{
	__int64 qpcNow = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
	double elapsed = (double)(qpcNow - m_lastLapStart) / (double)m_freq;
	return static_cast<float>(elapsed);
}

// ****************************************************************************
// ****************************************************************************
float Timer::LapMilliseconds()
{
	__int64 qpcNow = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&qpcNow);
	double elapsed = ((double)(qpcNow - m_lastLapStart) * 1000.0) / (double)m_freq;
	return static_cast<float>(elapsed);
}
} // namespace Helix