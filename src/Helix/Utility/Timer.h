#ifndef TIMER_H
#define TIMER_H

namespace Helix {
class Timer 
{
public:
	Timer();
	~Timer();

	void	Start();
	void	Stop();
	float	CurrentElapsedSeconds();
	float	CurrentElapsedMilliseconds();
	float	ElapsedSeconds();
	float	ElapsedMilliseconds();
	float	LapSeconds();
	float	LapMilliseconds();
	void	LapMark();
	void	Reset();
	bool	IsRunning() { return m_running; }

private:
	__int64		m_start;
	__int64		m_lastLapStart;
	__int64		m_freq;
	float		m_elapsed;
	bool		m_running;
};

} // namespace Helix
#endif // TIMER_H