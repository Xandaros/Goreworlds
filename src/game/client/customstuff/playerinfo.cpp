#include "playerinfo.h"


#include <game/generated/client_data.h>
#include <base/math.h>

#include <engine/graphics.h>
#include <game/client/render.h>





CPlayerInfo::CPlayerInfo()
{
	Reset();
}


void CPlayerInfo::Reset()
{
	m_UpdateTimer = 0;
	m_InUse = false;
	m_NextTeeSplatter = 0;
	
	for (int i = 0; i < MAX_TEESPLATTER; i++)
		m_aTeeSplatter[i].Reset();
	
	/*
	m_aWeaponSprite[WEAPON_HAMMER] = SPRITE_WEAPON_HAMMER01+rand()%2;
	m_aWeaponSprite[WEAPON_GUN] = SPRITE_WEAPON_GUN01+rand()%2;
	m_aWeaponSprite[WEAPON_SHOTGUN] = SPRITE_WEAPON_SHOTGUN01+rand()%2;
	m_aWeaponSprite[WEAPON_GRENADE] = SPRITE_WEAPON_GRENADE01+rand()%2;
	m_aWeaponSprite[WEAPON_RIFLE] = SPRITE_WEAPON_RIFLE01+rand()%2;
	m_aWeaponSprite[WEAPON_NINJA] = SPRITE_WEAPON_NINJA01+rand()%2;
	*/

	m_aWeaponSprite[WEAPON_HAMMER] = SPRITE_WEAPON_HAMMER01;
	m_aWeaponSprite[WEAPON_GUN] = SPRITE_WEAPON_GUN01;
	m_aWeaponSprite[WEAPON_SHOTGUN] = SPRITE_WEAPON_SHOTGUN01;
	m_aWeaponSprite[WEAPON_GRENADE] = SPRITE_WEAPON_GRENADE01;
	m_aWeaponSprite[WEAPON_RIFLE] = SPRITE_WEAPON_RIFLE01;
	m_aWeaponSprite[WEAPON_NINJA] = SPRITE_WEAPON_NINJA01;
}


void CPlayerInfo::Update(vec2 Pos)
{
	if (!m_InUse)
		Reset();
	
	m_InUse = true;
	m_UpdateTimer = 0;
	m_Pos = Pos;
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



void CPlayerInfo::Tick()
{
	for (int i = 0; i < MAX_TEESPLATTER; i++)
		m_aTeeSplatter[i].Tick();
	
	if (m_UpdateTimer++ > 5)
		m_InUse = false;
}

