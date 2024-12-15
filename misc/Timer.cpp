
#include "Timer.h"

CTimer::CTimer()
{
	m_stopped = true;
	m_inited = false;
	m_usingQPF = false;

	m_lastElapsedTime = 0.0;
	m_baseTime = 0.0;
	m_stopTime = 0.0;
	m_currSysTime = 0.0;
	m_currElapsedTime = 0.0;

	m_baseMilliTime = 0.0;
	m_currSysMilliTime = 0.0;
	m_currElapsedMilliTime = 0.0;

	m_QPFTicksPerSec = 0;
	m_QPFStopTime = 0;
	m_QPFLastElapsedTime = 0;
	m_QPFBaseTime = 0;
}

CTimer::~CTimer()
{
}

void CTimer::Start()
{
	if( !m_inited )
	{
		LARGE_INTEGER qwTicksPerSec;
		m_usingQPF = QueryPerformanceFrequency( &qwTicksPerSec );
		if( m_usingQPF )
			m_QPFTicksPerSec = qwTicksPerSec.QuadPart;

		if( m_usingQPF )
		{
			QueryPerformanceCounter( &m_QPFTime );
			m_QPFBaseTime = m_QPFTime.QuadPart;
			m_currSysTime = m_QPFBaseTime / ( double )m_QPFTicksPerSec;
			m_baseTime = m_currSysTime;

			m_currSysMilliTime = m_currSysTime * 1000.0;
			m_baseMilliTime = m_baseTime * 1000.0;
		}
		else
		{
			m_currSysTime = GetTickCount() * 0.001;
			m_baseTime = m_currSysTime;

			m_currSysMilliTime = m_currSysTime * 1000.0;
			m_baseMilliTime = m_baseTime * 1000.0;
		}

		m_inited = true;
	}

	if( m_usingQPF )
	{
		QueryPerformanceCounter( &m_QPFTime );
		m_currSysTime = m_QPFTime.QuadPart / ( double )m_QPFTicksPerSec;
		m_currSysMilliTime = m_currSysTime * 1000.0;

		m_QPFStopTime = 0;
		m_QPFLastElapsedTime = m_QPFTime.QuadPart;
	}
	else
	{
		m_currSysTime = GetTickCount() * 0.001;
		m_currSysMilliTime = m_currSysTime * 1000.0;

		m_stopTime = 0.0f;
		m_lastElapsedTime = m_currSysTime;
	}

	m_stopped = false;
}

void CTimer::Stop()
{
	if( m_stopped ) return;

	if( m_usingQPF )
	{
		QueryPerformanceCounter( &m_QPFTime );
		m_currSysTime = m_QPFTime.QuadPart / ( double )m_QPFTicksPerSec;
		m_currSysMilliTime = m_currSysTime * 1000.0;

		m_QPFStopTime = m_QPFTime.QuadPart;
		m_QPFLastElapsedTime = m_QPFTime.QuadPart;
	}
	else
	{
		m_currSysTime = GetTickCount() * 0.001;
		m_currSysMilliTime = m_currSysTime * 1000.0;

		m_stopTime = m_currSysTime;
		m_lastElapsedTime = m_currSysTime;
	}

	m_stopped = true;
}

void CTimer::Advance()
{
	if( m_usingQPF )
		m_QPFStopTime += m_QPFTicksPerSec / 10;
	else
		m_stopTime += 0.1f;
}

void CTimer::Reset()
{
	if( m_usingQPF )
	{
		QueryPerformanceCounter( &m_QPFTime );
		m_currSysTime = m_QPFTime.QuadPart / ( double )m_QPFTicksPerSec;
		m_currSysMilliTime = m_currSysTime * 1000.0;

		m_QPFBaseTime = m_QPFTime.QuadPart;
		m_QPFLastElapsedTime = m_QPFTime.QuadPart;
		m_QPFStopTime = 0;

		m_baseTime = m_QPFBaseTime / ( double )m_QPFTicksPerSec;
	}
	else
	{
		m_currSysTime = GetTickCount() * 0.001;
		m_currSysMilliTime = m_currSysTime * 1000.0;

		m_baseTime = m_currSysTime;
		m_lastElapsedTime = m_currSysTime;
		m_stopTime = 0.0;
	}

	m_stopped = false;
}

double CTimer::Tick()
{
	if( m_stopped )
	{
		if( m_usingQPF )
		{
			m_currSysTime = m_QPFStopTime / ( double )m_QPFTicksPerSec;
			m_currSysMilliTime = m_currSysTime * 1000.0;
		}
		else
		{
			m_currSysTime = m_stopTime;
			m_currSysMilliTime = m_currSysTime * 1000.0;
		}
	}
	else
	{
		if( m_usingQPF )
		{
			QueryPerformanceCounter( &m_QPFTime );
			m_currSysTime = m_QPFTime.QuadPart / ( double )m_QPFTicksPerSec;
			m_currSysMilliTime = m_currSysTime * 1000.0;

			m_currElapsedTime = ( double )( m_QPFTime.QuadPart - m_QPFLastElapsedTime ) / ( double )m_QPFTicksPerSec;
			m_QPFLastElapsedTime = m_QPFTime.QuadPart;

			m_currElapsedMilliTime = m_currElapsedTime * 1000.0;
		}
		else
		{
			m_currSysTime = GetTickCount() * 0.001;
			m_currSysMilliTime = m_currSysTime * 1000.0;

			m_currElapsedTime = ( double )( m_currSysTime - m_lastElapsedTime );
			m_lastElapsedTime = m_currSysTime;

			m_currElapsedMilliTime = m_currElapsedTime * 1000.0;
		}
	}
	return ( float )m_currElapsedTime;
}

double CTimer::GetAbsoluteTime()
{
	if( m_stopped )
	{
		if( m_usingQPF )
		{
			return ( m_QPFStopTime / ( double )m_QPFTicksPerSec );
		}
		else
		{
			return ( m_stopTime );
		}
	}
	else
	{
		if( m_usingQPF )
		{
			QueryPerformanceCounter( &m_QPFTime );

			return ( m_QPFTime.QuadPart / ( double )m_QPFTicksPerSec );
		}
		else
		{
			return ( GetTickCount() * 0.001 );
		}
	}
}