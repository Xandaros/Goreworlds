#include "customstuff.h"



CCustomStuff::CCustomStuff()
{
	Reset();
}


void CCustomStuff::Reset()
{
	m_LastUpdate = time_get();
	m_Tick = 0;
	
	for (int i = 0; i < MAX_CLIENTS; i++)
		m_aPlayerInfo[i].Reset();
}



void CCustomStuff::Tick()
{
	m_Tick++;
	
	for (int i = 0; i < MAX_CLIENTS; i++)
		m_aPlayerInfo[i].Tick();
}


void CCustomStuff::Update()
{
	int64 currentTime = time_get();
	if ((currentTime-m_LastUpdate > time_freq()) || (m_LastUpdate == 0))
		m_LastUpdate = currentTime;
		
	int step = time_freq()/60;
	
	for (;m_LastUpdate < currentTime; m_LastUpdate += step)
	{
		Tick();
	}
	
}