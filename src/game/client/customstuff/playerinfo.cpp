#include "playerinfo.h"


#include <game/generated/client_data.h>
#include <base/math.h>

#include <engine/graphics.h>
#include <game/client/render.h>
#include <engine/shared/config.h>

#include <game/client/customstuff/tracer.h>
#include <game/client/customstuff/meleeweapon.h>




CPlayerInfo::CPlayerInfo()
{
	m_pTracer = NULL;
	Reset();
}


void CPlayerInfo::Reset()
{
	m_UpdateTimer = 0;
	m_InUse = false;
	m_NextTeeSplatter = 0;
	
	for (int i = 0; i < MAX_TEESPLATTER; i++)
		m_aTeeSplatter[i].Reset();
	
	if (g_Config.m_GoreRandomWeapons)
	{	
		m_aWeaponSprite[WEAPON_HAMMER] = SPRITE_WEAPON_HAMMER01+rand()%2;
		m_aWeaponSprite[WEAPON_GUN] = SPRITE_WEAPON_GUN01+rand()%2;
		m_aWeaponSprite[WEAPON_SHOTGUN] = SPRITE_WEAPON_SHOTGUN01+rand()%3;
		m_aWeaponSprite[WEAPON_GRENADE] = SPRITE_WEAPON_GRENADE01+rand()%3;
		m_aWeaponSprite[WEAPON_RIFLE] = SPRITE_WEAPON_RIFLE01+rand()%4;
		m_aWeaponSprite[WEAPON_NINJA] = SPRITE_WEAPON_NINJA01+rand()%2;
	}
	else
	{
		m_aWeaponSprite[WEAPON_HAMMER] = SPRITE_WEAPON_HAMMER01;
		m_aWeaponSprite[WEAPON_GUN] = SPRITE_WEAPON_GUN01;
		m_aWeaponSprite[WEAPON_SHOTGUN] = SPRITE_WEAPON_SHOTGUN01;
		m_aWeaponSprite[WEAPON_GRENADE] = SPRITE_WEAPON_GRENADE01;
		m_aWeaponSprite[WEAPON_RIFLE] = SPRITE_WEAPON_RIFLE01;
		m_aWeaponSprite[WEAPON_NINJA] = SPRITE_WEAPON_NINJA01;
	}
	
	// reset bounciness
	m_FeetOffset = vec2(0, 0);
	m_FeetOffsetVel = vec2(0, 0);
	
	m_WeaponRecoil = vec2(0, 0);
	m_WeaponRecoilVel = vec2(0, 0);
	
	m_Weapon2Recoil = vec2(0, 0);
	m_Weapon2RecoilVel = vec2(0, 0);
	
	// reset tracer
	if (!m_pTracer)
		m_pTracer = new CTracer();
	else
		m_pTracer->Clean();

	// reset weapon
	if (!m_pMeleeWeapon)
		m_pMeleeWeapon = new CMeleeWeapon();
	else
		m_pMeleeWeapon->Reset();
	
	m_UseCustomMeleeWeapon = false;
}


void CPlayerInfo::Update(vec2 Pos)
{
	if (!m_InUse)
		Reset();
	
	m_InUse = true;
	m_UpdateTimer = 0;
	m_Pos = Pos;
	
	if (m_pMeleeWeapon)
		m_pMeleeWeapon->m_PlayerPos = Pos;
}



void CPlayerInfo::AddTeeSplatter(float Angle)
{
	m_aTeeSplatter[m_NextTeeSplatter].m_Life = m_aTeeSplatter[m_NextTeeSplatter].m_StartLife;
	m_aTeeSplatter[m_NextTeeSplatter].m_Angle = Angle;
	m_aTeeSplatter[m_NextTeeSplatter].m_Sprite = SPRITE_TEESPLATTER01 + rand()%4;
	
	if (rand()%10 < 5)
		m_aTeeSplatter[m_NextTeeSplatter].m_Flip = -1;
	else
		m_aTeeSplatter[m_NextTeeSplatter].m_Flip = 1;
	
	if (++m_NextTeeSplatter >= MAX_TEESPLATTER)
		m_NextTeeSplatter = 0;
}


void CPlayerInfo::RenderTeeSplatter(IGraphics *Graphics, CRenderTools *RenderTools)
{
	Graphics->BlendNormal();
	//gfx_blend_additive();
	Graphics->TextureSet(g_pData->m_aImages[IMAGE_TEESPLATTER].m_Id);
	Graphics->QuadsBegin();
	
	for (int i = 0; i < MAX_TEESPLATTER; i++)
	{
		if (m_aTeeSplatter[i].m_Life > 0)
		{
			RenderTools->SelectSprite(m_aTeeSplatter[i].m_Sprite);
			float a = float(m_aTeeSplatter[i].m_Life) / float(m_aTeeSplatter[i].m_StartLife);
			float Size = 42;

			Graphics->QuadsSetRotation(m_aTeeSplatter[i].m_Angle);

			//Graphics->SetColor(1.0f-a*0.6f, 1.0f-a*0.6f, 1.0f-a*0.6f, a);
			Graphics->SetColor(0.3f+a*0.7f, 0.3f+a*0.7f, 0.3f+a*0.7f, a);

			IGraphics::CQuadItem QuadItem(m_Pos.x, m_Pos.y, Size, Size);
			Graphics->QuadsDraw(&QuadItem, 1);
		}
	}
	
	Graphics->QuadsEnd();
	Graphics->BlendNormal();
}



void CPlayerInfo::PhysicsTick(vec2 PlayerVel, vec2 PrevVel)
{
	if (!g_Config.m_GoreBouncyTee)
	{
		m_FeetOffsetVel = vec2(0, 0);
		m_FeetOffset = vec2(0, 0);
		return;
	}
	
	//float b = 1.5f - g_Config.m_GoreTeeBounciness / 100.0f;
	
	// feet
	m_FeetOffsetVel.x -= ((PlayerVel.x-PrevVel.x)/800.0f);
	m_FeetOffsetVel.x -= (m_FeetOffset.x / 16.0f);
	m_FeetOffsetVel.x -= m_FeetOffsetVel.x*0.15f;

	if (PlayerVel.y == 0) //m_Jumped
		m_FeetOffsetVel.x -= m_FeetOffsetVel.x*0.15f;

	if (PlayerVel.y < 0)
		m_FeetOffsetVel.y -= (PlayerVel.y/1400.0f);

	m_FeetOffsetVel.y -= (m_FeetOffset.y / 4.0f);
	m_FeetOffsetVel.y -= m_FeetOffsetVel.y*0.15f;

	m_FeetOffset += m_FeetOffsetVel;
	
	
	// weapon recoil
 	m_WeaponRecoilVel.x -= m_WeaponRecoil.x / 6.0f;
	m_WeaponRecoilVel.y -= m_WeaponRecoil.y / 6.0f;
	m_WeaponRecoilVel *= 0.82f;
			
	m_WeaponRecoil += m_WeaponRecoilVel;
	
	m_Weapon2RecoilVel.x += (PlayerVel.x-PrevVel.x)/2000.0f;
	m_Weapon2RecoilVel.y += (PlayerVel.y-PrevVel.y)/2000.0f;
		
	m_Weapon2RecoilVel.x -= m_Weapon2Recoil.x / 12.0f;
	m_Weapon2RecoilVel.y -= m_Weapon2Recoil.y / 12.0f;
		
	m_Weapon2RecoilVel *= 0.8f;

	m_Weapon2Recoil += m_Weapon2RecoilVel;
	
	if (g_Config.m_GoreTracer)
	{
		float a = -GetAngle(PlayerVel);
		float s = 20;
		
		m_pTracer->SetTracePos(m_Pos+vec2(sin(a)*s, cos(a)*s), m_Pos-vec2(sin(a)*s, cos(a)*s));
		m_pTracer->Update(true);
	}
	
	if (m_pMeleeWeapon)
		m_pMeleeWeapon->Tick();
}
	
	
void CPlayerInfo::UpdatePhysics(vec2 PlayerVel, vec2 PrevVel)
{
	int64 currentTime = time_get();
	if ((currentTime-m_LastUpdate > time_freq()) || (m_LastUpdate == 0))
		m_LastUpdate = currentTime;
		
	int step = time_freq()/60;
	
	for (;m_LastUpdate < currentTime; m_LastUpdate += step)
		PhysicsTick(PlayerVel, PrevVel);
	
}
	
void CPlayerInfo::Tick()
{
	for (int i = 0; i < MAX_TEESPLATTER; i++)
		m_aTeeSplatter[i].Tick();
	
	if (m_UpdateTimer++ > 5)
	{
		m_InUse = false;
		if (m_pTracer)
			m_pTracer->Clean();
	}
}

