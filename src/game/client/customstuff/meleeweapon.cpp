#include "meleeweapon.h"

#include <math.h>
#include <base/math.h>

#include <engine/graphics.h>
#include <game/generated/client_data.h>
#include <game/generated/protocol.h>

#include <game/client/render.h>

#include <game/client/customstuff/tracer.h>

#define RAD 0.017453292519943295769236907684886f


enum MeleeWeapon
{
	MELEEWEAPON_CLEAVER,
	MELEEWEAPON_KNIFE	
};

enum NextMove
{
	STAB,
	STRIKEDOWN,
	STRIKEUP,
};

enum State
{
	IDLE,
	HIT,
	AFTERHIT,
	RETURN
};

enum AttackPattern
{
	UPDOWNSTRIKE,
	DOWNSTRIKE,
};



CMeleeWeapon::CMeleeWeapon()
{
	m_pTracer = NULL;
	Reset();
	SetSword();
}


void CMeleeWeapon::Reset()
{	
	m_Angle = 0;
	m_Pos = vec2(0, 0);
	m_TargetPos = vec2(0, 0);

	m_Offset = vec2(-14, -5);
	m_Size = vec2(48, 24);
	
	m_Steps = 0;
	m_FlipY = 1;
	
	m_LatestAttackTick = 0;
	
	m_State = IDLE;
	m_StateTick = 0;
		
	m_Dir = 1;
	m_TargetDir = 1;
		
	// reset tracer
	if (!m_pTracer)
		m_pTracer = new CTracer();
	else
		m_pTracer->Clean();
	
	switch (rand()%3)
	{
	case 0:
		SetHammer();
		break;
	case 1:
		SetCleaver();
		break;
	case 2:
		SetSword();
		break;
	default:
		SetHammer();
		break;
	};
}


void CMeleeWeapon::SetCleaver()
{
	//Reset();
	
	m_Sprite = SPRITE_WEAPON_CLEAVER;
	m_NextMove = STRIKEDOWN;
	
	m_AttackPattern = DOWNSTRIKE;
	
	m_TracerLen = 12;
	m_TracerOffset = vec2(20, -7);
	
	m_Offset = vec2(-14, -5);
	m_Size = vec2(48, 24);
	
	m_pTracer->SetColor(vec4(0.5f, 0.5f, 0.5f, 0.4f), vec4(0.7f, 0.7f, 0.7f, 0.7f));
	
	m_DefaultAngle = -25*RAD;
	m_DefaultPos = vec2(-10, -15);
	
	m_Angle = m_DefaultAngle;
	m_Pos = m_DefaultPos;
}


void CMeleeWeapon::SetHammer()
{
	//Reset();
	
	m_Sprite = SPRITE_WEAPON_HAMMER01;
	m_NextMove = STRIKEDOWN;
	
	m_AttackPattern = DOWNSTRIKE;
	
	m_TracerLen = 10;
	m_TracerOffset = vec2(24, 0);
	
	m_Offset = vec2(-8, -1);
	m_Size = vec2(64, 48);
	
	m_pTracer->SetColor(vec4(0.45f, 0.3f, 0.1f, 0.6f), vec4(0.45f, 0.3f, 0.1f, 0.8f));
	
	m_DefaultAngle = -25*RAD;
	m_DefaultPos = vec2(-10, -15);
	
	m_Angle = m_DefaultAngle;
	m_Pos = m_DefaultPos;
}


void CMeleeWeapon::SetSword()
{
	//Reset();
	
	m_Sprite = SPRITE_WEAPON_NINJA01;
	m_NextMove = STRIKEDOWN;
	
	m_AttackPattern = UPDOWNSTRIKE;
	
	m_TracerLen = 21;
	m_TracerOffset = vec2(31, 0);
	
	m_Offset = vec2(-17, -0);
	m_Size = vec2(96, 32);
	
	m_pTracer->SetColor(vec4(0, 0.5f, 0, 0.4f), vec4(0, 0.75f, 0, 0.7f));
	
	m_DefaultAngle = -25*RAD;
	m_DefaultPos = vec2(-10, -15);
	
	m_Angle = m_DefaultAngle;
	m_Pos = m_DefaultPos;
}



void CMeleeWeapon::SetTarget(vec2 Pos, float Angle)
{
	m_TargetPos = Pos;
	m_TargetAngle = Angle;
	
	m_PosStep.x = (m_TargetPos.x - m_Pos.x) / 3.0f;
	m_PosStep.y = (m_TargetPos.y - m_Pos.y) / 3.0f;
	
	m_AngleStep = (m_TargetAngle - m_Angle) / 3.0f;
	
	m_Steps = 3;
}

	
void CMeleeWeapon::Hit(int AttackTick)
{
	if (AttackTick > m_LatestAttackTick && m_State != HIT)
	{
		m_LatestAttackTick = AttackTick;
		m_State = HIT;
		m_StateTick = 0;
		
		if (m_NextMove == STRIKEDOWN)
		{
			m_FlipY = 1;
			if (m_AttackPattern == UPDOWNSTRIKE)
				m_NextMove = STRIKEUP;
			
			if (m_AttackPattern == DOWNSTRIKE)
				SetTarget(vec2(7+frandom()*7, frandom()*7), (90+frandom()*20)*RAD);
			else
				SetTarget(vec2(7+frandom()*10, 10+frandom()*10), (100+frandom()*40)*RAD);
		}
		else if (m_NextMove == STRIKEUP)
		{
			m_FlipY = -1;
			m_NextMove = STRIKEDOWN;
			SetTarget(vec2(frandom()*10, -20-frandom()*15), (60-frandom()*50)*RAD);
		}
	}
}



void CMeleeWeapon::Tick()
{
	m_LeaveTrace = false;
	
	// move & turn
	if (m_Steps > 0)
	{
		m_Steps--;
		
		m_Pos += m_PosStep;
		m_Angle += m_AngleStep;
	}
	
	
	if (m_State == HIT)
	{
		m_StateTick++;
		m_LeaveTrace = true;
		
		if (m_StateTick > 3)
		{
			m_State = AFTERHIT;
			m_StateTick = 0;
		}
	}
	else
	if (m_State == AFTERHIT)
	{
		m_StateTick++;
		
		if (m_Dir != m_TargetDir || m_AttackPattern == DOWNSTRIKE)
		{
			m_StateTick = 99;
			m_Dir = m_TargetDir;
		}
		
		if (m_StateTick > 10)
		{
			m_State = IDLE;
			m_StateTick = 0;
			
			m_NextMove = STRIKEDOWN;
			m_FlipY = 1;
			
			SetTarget(m_DefaultPos, m_DefaultAngle);
		}
	}
	else
	{
		m_Dir = m_TargetDir;
	}

	float s = 16;

	if (m_pTracer)
	{
		vec2 Pos1 = m_Pos+vec2(sin(-m_Angle)*m_TracerLen, cos(-m_Angle)*m_TracerLen);
		vec2 Pos2 = m_Pos-vec2(sin(-m_Angle)*m_TracerLen, cos(-m_Angle)*m_TracerLen);
		
		Pos1.x *= m_Dir;
		Pos2.x *= m_Dir;
		
		vec2 Offset;
		Offset.x = -sin(-m_Angle)*m_TracerOffset.x;
		Offset.y = -cos(-m_Angle)*m_TracerOffset.x;
		
		Offset.x += sin(-m_Angle-90*RAD)*m_TracerOffset.y*m_FlipY;
		Offset.y += cos(-m_Angle-90*RAD)*m_TracerOffset.y*m_FlipY;
		
		Offset.x *= m_Dir;
		
		m_pTracer->SetTracePos(m_PlayerPos+Pos1+Offset, m_PlayerPos+Pos2+Offset);
		//m_pTracer->Update(true);
		m_pTracer->Update(m_LeaveTrace);
	}
}








