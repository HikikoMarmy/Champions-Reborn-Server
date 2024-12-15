#pragma once

#include <Windows.h>

class CTimer {
	int  m_stopped;
	int  m_inited;
	int  m_usingQPF;

	double  m_lastElapsedTime;
	double  m_baseTime;
	double  m_stopTime;
	double  m_currSysTime;
	double  m_currElapsedTime;
	double  m_baseMilliTime;
	double  m_currSysMilliTime;
	double  m_currElapsedMilliTime;

	long long  m_QPFTicksPerSec;
	long long  m_QPFStopTime;
	long long  m_QPFLastElapsedTime;
	long long  m_QPFBaseTime;

	LARGE_INTEGER m_QPFTime;

public:

	CTimer();
	 ~CTimer();

	void  Start();
	void  Stop();
	void  Advance();
	void  Reset();
	double Tick();
	inline double GetAppTime()
	{
		return ( m_currSysTime - m_baseTime );
	}
	inline double GetElapsedTime()
	{
		return m_currElapsedTime;
	}
	inline double GetSysTime()
	{
		return m_currSysTime;
	}
	inline double GetAppMilliTime()
	{
		return ( m_currSysMilliTime - m_baseMilliTime );
	}
	inline double GetElapsedMilliTime()
	{
		return m_currElapsedMilliTime;
	}

	double GetAbsoluteTime();
};
