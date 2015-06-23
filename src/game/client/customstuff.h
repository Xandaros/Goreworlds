#ifndef GAME_CLIENT_CUSTOMSTUFF_H
#define GAME_CLIENT_CUSTOMSTUFF_H

#include <base/system.h>

#include <game/generated/protocol.h>
#include <game/generated/client_data.h>
#include <game/client/gameclient.h>

#include "customstuff/playerinfo.h"



class CCustomStuff
{
private:
	// local tick for helping rendering physics & stuff
	int m_Tick;
	
	int64 m_LastUpdate;
	
	vec2 m_CameraTargetCenter;
	vec2 m_CameraCenter;
	
public:
	int LocalTick(){ return m_Tick; }
	
	void SetCameraTarget(vec2 Center){ m_CameraTargetCenter = Center; }
	vec2 GetCameraCenter(){ return m_CameraCenter; }
	
	CCustomStuff();
	
	void Reset();
	
	void Tick();
	void Update();
	
	CPlayerInfo m_aPlayerInfo[MAX_CLIENTS];
	
	// gamemode Killing Floor
	bool m_KF;
};



#endif