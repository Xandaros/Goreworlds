#ifndef GAME_CLIENT_COMPONENTS_MELEEWEAPON_H
#define GAME_CLIENT_COMPONENTS_MELEEWEAPON_H

#include <base/vmath.h>
#include <base/math.h>
#include <base/system.h>


#define RAD 0.017453292519943295769236907684886f


class CMeleeWeapon
{
private:
	vec2 m_TargetPos;
	float m_TargetAngle;
	
	vec2 m_DefaultPos;
	float m_DefaultAngle;
	
	vec2 m_PosStep;
	float m_AngleStep;
	
	int m_Steps;

	bool m_LeaveTrace;
	
	int m_AttackPattern;
	
public:
	class CTracer *m_pTracer;
	
	int m_TargetDir;
	int m_Dir;
	
	void SetSword();
	void SetCleaver();
	void SetHammer();
	
	int m_FlipY;
	
	vec2 m_TracerOffset;
	float m_TracerLen;
	
	int m_Sprite;
	int m_State;
	
	int m_StateTick;
	
	vec2 m_Pos;
	float m_Angle;
	
	// for tracer
	vec2 m_PlayerPos;

	vec2 m_Offset;
	vec2 m_Size;
	
	int m_NextMove;
	
	int m_LatestAttackTick;
	
	CMeleeWeapon();
	
	void Reset();
	void Tick();
	void Hit(int AttackTick);
	
	void SetTarget(vec2 Pos, float Angle);
};		

#endif