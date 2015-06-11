/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "SDL.h"

#include <base/system.h>
#include <engine/shared/config.h>
#include <engine/graphics.h>
#include <engine/input.h> 
#include <engine/keys.h>

#include "input.h"

//print >>f, "int inp_key_code(const char *key_name) { int i; if (!strcmp(key_name, \"-?-\")) return -1; else for (i = 0; i < 512; i++) if (!strcmp(key_strings[i], key_name)) return i; return -1; }"

// this header is protected so you don't include it from anywere
#define KEYS_INCLUDE
#include "keynames.h"
#undef KEYS_INCLUDE

bool UseGamepad = false;

void CInput::AddEvent(int Unicode, int Key, int Flags)
{
	if(m_NumEvents != INPUT_BUFFER_SIZE)
	{
		m_aInputEvents[m_NumEvents].m_Unicode = Unicode;
		m_aInputEvents[m_NumEvents].m_Key = Key;
		m_aInputEvents[m_NumEvents].m_Flags = Flags;
		m_NumEvents++;
	}
}

CInput::CInput()
{
	mem_zero(m_aInputCount, sizeof(m_aInputCount));
	mem_zero(m_aInputState, sizeof(m_aInputState));

	m_InputCurrent = 0;
	m_InputGrabbed = 0;
	m_InputDispatched = false;

	m_LastRelease = 0;
	m_ReleaseDelta = -1;

	m_NumEvents = 0;

	m_pGamepad = NULL;
}

void CInput::Init()
{
	m_pGraphics = Kernel()->RequestInterface<IEngineGraphics>();
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}





void CInput::MouseRelative(float *x, float *y)
{
	if (UseGamepad)
	{
		*x = m_Axis1.x;
		*y = m_Axis1.y;
		return;
	}

	int nx = 0, ny = 0;
	float Sens = g_Config.m_InpMousesens/100.0f;

	if(g_Config.m_InpGrab)
		SDL_GetRelativeMouseState(&nx, &ny);
	else
	{
		if(m_InputGrabbed)
		{
			SDL_GetMouseState(&nx,&ny);
			SDL_WarpMouse(Graphics()->ScreenWidth()/2,Graphics()->ScreenHeight()/2);
			nx -= Graphics()->ScreenWidth()/2; ny -= Graphics()->ScreenHeight()/2;
		}
	}

	*x = nx*Sens;
	*y = ny*Sens;
}

void CInput::MouseModeAbsolute()
{
	SDL_ShowCursor(1);
	m_InputGrabbed = 0;
	if(g_Config.m_InpGrab)
		SDL_WM_GrabInput(SDL_GRAB_OFF);
}

void CInput::MouseModeRelative()
{
	SDL_ShowCursor(0);
	m_InputGrabbed = 1;
	if(g_Config.m_InpGrab)
		SDL_WM_GrabInput(SDL_GRAB_ON);
}

int CInput::MouseDoubleClick()
{
	if(m_ReleaseDelta >= 0 && m_ReleaseDelta < (time_freq() >> 2))
	{
		m_LastRelease = 0;
		m_ReleaseDelta = -1;
		return 1;
	}
	return 0;
}

void CInput::ClearKeyStates()
{
	mem_zero(m_aInputState, sizeof(m_aInputState));
	mem_zero(m_aInputCount, sizeof(m_aInputCount));
}

int CInput::KeyState(int Key)
{
	return m_aInputState[m_InputCurrent][Key];
}


bool rapidFire = false;
bool mouseLeft, rapidState;

int rapidCount = 0;



int CInput::Update()
{
	if (UseGamepad)
	{
		if (!m_pGamepad)
			m_pGamepad = SDL_JoystickOpen(0);
	}
	else
	{
		//if (m_pGamepad)
		//	SDL_JoystickClose(m_pGamepad);
	}

	if(m_InputGrabbed && !Graphics()->WindowActive())
		MouseModeAbsolute();

	/*if(!input_grabbed && Graphics()->WindowActive())
		Input()->MouseModeRelative();*/

	if(m_InputDispatched)
	{
		// clear and begin count on the other one
		m_InputCurrent^=1;
		mem_zero(&m_aInputCount[m_InputCurrent], sizeof(m_aInputCount[m_InputCurrent]));
		mem_zero(&m_aInputState[m_InputCurrent], sizeof(m_aInputState[m_InputCurrent]));
		m_InputDispatched = false;
	}

	{
		int i;
		Uint8 *pState = SDL_GetKeyState(&i);
		if(i >= KEY_LAST)
			i = KEY_LAST-1;
		mem_copy(m_aInputState[m_InputCurrent], pState, i);
	}

	// these states must always be updated manually because they are not in the GetKeyState from SDL
	int i = SDL_GetMouseState(NULL, NULL);
	if(i&SDL_BUTTON(1)) m_aInputState[m_InputCurrent][KEY_MOUSE_1] = 1; // 1 is left	
	if(i&SDL_BUTTON(3)) m_aInputState[m_InputCurrent][KEY_MOUSE_2] = 1; // 3 is right
	if(i&SDL_BUTTON(2)) m_aInputState[m_InputCurrent][KEY_MOUSE_3] = 1; // 2 is middle
	if(i&SDL_BUTTON(4)) m_aInputState[m_InputCurrent][KEY_MOUSE_4] = 1;
	if(i&SDL_BUTTON(5)) m_aInputState[m_InputCurrent][KEY_MOUSE_5] = 1;
	if(i&SDL_BUTTON(6)) m_aInputState[m_InputCurrent][KEY_MOUSE_6] = 1;
	if(i&SDL_BUTTON(7)) m_aInputState[m_InputCurrent][KEY_MOUSE_7] = 1;
	if(i&SDL_BUTTON(8)) m_aInputState[m_InputCurrent][KEY_MOUSE_8] = 1;

	// update gamepad status
	if (m_pGamepad && UseGamepad)
	{
		m_Axis1.x = SDL_JoystickGetAxis(m_pGamepad, 4) / 100;
		m_Axis1.y = SDL_JoystickGetAxis(m_pGamepad, 3) / 100;
		
		int Axis2 = SDL_JoystickGetAxis(m_pGamepad, 0) / 100;
		int Jump = SDL_JoystickGetAxis(m_pGamepad, 1) / 100;
		int Hook = SDL_JoystickGetAxis(m_pGamepad, 2) / 100;
		//int Shoot = SDL_JoystickGetAxis(m_pGamepad, 5) / 100;
		
		/*
		if (Hook > 110 && !m_Shoot)
		{
			m_Shoot = true;
			m_aInputCount[m_InputCurrent][KEY_MOUSE_1].m_Presses++;
			m_aInputState[m_InputCurrent][KEY_MOUSE_1] = 1;
			AddEvent(0, KEY_MOUSE_1, IInput::FLAG_PRESS);
			rapidCount = 0;
			mouseLeft = true;
			rapidState = true;
		}
		
		if (Hook < 100 && m_Shoot)
		{
			m_Shoot = false;
			m_aInputCount[m_InputCurrent][KEY_MOUSE_1].m_Presses++;
			AddEvent(0, KEY_MOUSE_1, IInput::FLAG_RELEASE);
			mouseLeft = false;
		}	
		*/
		
		if (Hook < -110 && !m_Hook)
		{
			m_Hook = true;
			m_aInputCount[m_InputCurrent][KEY_MOUSE_2].m_Presses++;
			m_aInputState[m_InputCurrent][KEY_MOUSE_2] = 1;
			AddEvent(0, KEY_MOUSE_2, IInput::FLAG_PRESS);
		}
		
		if (Hook > -100 && m_Hook)
		{
			m_Hook = false;
			m_aInputCount[m_InputCurrent][KEY_MOUSE_2].m_Presses++;
			AddEvent(0, KEY_MOUSE_2, IInput::FLAG_RELEASE);
		}		
		
		///*
		if (Jump < -100 && !m_Jump)
		{
			m_Jump = true;
			m_aInputCount[m_InputCurrent][KEY_SPACE].m_Presses++;
			m_aInputState[m_InputCurrent][KEY_SPACE] = 1;
			AddEvent(0, KEY_SPACE, IInput::FLAG_PRESS);
		}
		
		if (Jump > -90 && m_Jump)
		{
			m_Jump = false;
			m_aInputCount[m_InputCurrent][KEY_SPACE].m_Presses++;
			AddEvent(0, KEY_SPACE, IInput::FLAG_RELEASE);
		}
		//*/
		
			
		if (Axis2 <= -100 && !m_Left)
		{
			m_Left = true;
			m_aInputCount[m_InputCurrent]['a'].m_Presses++;
			m_aInputState[m_InputCurrent]['a'] = 1;
			AddEvent(0, 'a', IInput::FLAG_PRESS);
		}
		
		if (Axis2 >= 100 && !m_Right)
		{
			m_Right = true;
			m_aInputCount[m_InputCurrent]['d'].m_Presses++;
			m_aInputState[m_InputCurrent]['d'] = 1;
			AddEvent(0, 'd', IInput::FLAG_PRESS);
		}
		
		if (Axis2 > -100 && m_Left)
		{
			m_Left = false;
			m_aInputCount[m_InputCurrent]['a'].m_Presses++;
			AddEvent(0, 'a', IInput::FLAG_RELEASE);
		}
		
		if (Axis2 < 100 && m_Right)
		{
			m_Right = false;
			m_aInputCount[m_InputCurrent]['d'].m_Presses++;
			AddEvent(0, 'd', IInput::FLAG_RELEASE);
		}
	}

	
	
	
	
	{
		SDL_Event Event;

		while(SDL_PollEvent(&Event))
		{
			int Key = -1;
			int Action = IInput::FLAG_PRESS;
			switch (Event.type)
			{
				// handle keys
				case SDL_KEYDOWN:
					// uncomment to enable gamepad
					/*if (Event.key.keysym.sym == KEY_RCTRL)
					{
						UseGamepad = !UseGamepad;
						break;
					}*/
				
					if (UseGamepad &&
						Event.key.keysym.sym != KEY_RETURN &&
						Event.key.keysym.sym != KEY_ESCAPE &&
						Event.key.keysym.sym != KEY_UP &&
						Event.key.keysym.sym != KEY_DOWN)
							break;
							
					// skip private use area of the BMP(contains the unicodes for keyboard function keys on MacOS)
					if(Event.key.keysym.unicode < 0xE000 || Event.key.keysym.unicode > 0xF8FF)	// ignore_convention
						AddEvent(Event.key.keysym.unicode, 0, 0); // ignore_convention
					Key = Event.key.keysym.sym; // ignore_convention
					break;
				case SDL_KEYUP:
					if (UseGamepad &&
						Event.key.keysym.sym != KEY_RETURN &&
						Event.key.keysym.sym != KEY_ESCAPE &&
						Event.key.keysym.sym != KEY_UP &&
						Event.key.keysym.sym != KEY_DOWN)
							break;
					Action = IInput::FLAG_RELEASE;
					Key = Event.key.keysym.sym; // ignore_convention
					break;

				// handle mouse buttons
				case SDL_MOUSEBUTTONUP:
					Action = IInput::FLAG_RELEASE;

					if(Event.button.button == 1) // ignore_convention
					{
						m_ReleaseDelta = time_get() - m_LastRelease;
						m_LastRelease = time_get();
					}

					// fall through
				case SDL_MOUSEBUTTONDOWN:
					if(Event.button.button == SDL_BUTTON_LEFT) Key = KEY_MOUSE_1; // ignore_convention
					if(Event.button.button == SDL_BUTTON_RIGHT) Key = KEY_MOUSE_2; // ignore_convention
					if(Event.button.button == SDL_BUTTON_MIDDLE) Key = KEY_MOUSE_3; // ignore_convention
					if(Event.button.button == SDL_BUTTON_WHEELUP) Key = KEY_MOUSE_WHEEL_UP; // ignore_convention
					if(Event.button.button == SDL_BUTTON_WHEELDOWN) Key = KEY_MOUSE_WHEEL_DOWN; // ignore_convention
					if(Event.button.button == 6) Key = KEY_MOUSE_6; // ignore_convention
					if(Event.button.button == 7) Key = KEY_MOUSE_7; // ignore_convention
					if(Event.button.button == 8) Key = KEY_MOUSE_8; // ignore_convention
					break;				
					
					
					// gamepad
				case SDL_JOYBUTTONUP:
					Action = IInput::FLAG_RELEASE;

					if(Event.button.button == 1) // ignore_convention
					{
						m_ReleaseDelta = time_get() - m_LastRelease;
						m_LastRelease = time_get();
					}	
				
					// fall through
				case SDL_JOYBUTTONDOWN:
					//if(Event.button.button == 0) Key = KEY_MOUSE_1;
					//if(Event.button.button == 3) Key = KEY_MOUSE_2;
					if(Event.button.button == 0) Key = KEY_MOUSE_WHEEL_UP;
					if(Event.button.button == 4) Key = KEY_MOUSE_1;
					if(Event.button.button == 3) Key = KEY_MOUSE_WHEEL_DOWN;
					if(Event.button.button == 2) Key = KEY_LSHIFT;
					//if(Event.button.button == 1) Key = KEY_SPACE;
					break;

				// other messages
				case SDL_QUIT:
					return 1;
			}

			
			//Rapidfire
			if (Key == KEY_MOUSE_1)// && !UseGamepad)
			{
				if (Action == IInput::FLAG_RELEASE)
				{
					mouseLeft = false;
					//rapidState = false;
				}
				else
				{
					rapidCount = 0;
					mouseLeft = true;
					rapidState = true;
				}
			}
			
			
			//
			if(Key != -1)
			{
				m_aInputCount[m_InputCurrent][Key].m_Presses++;
				if(Action == IInput::FLAG_PRESS)
					m_aInputState[m_InputCurrent][Key] = 1;
				AddEvent(0, Key, Action);
			}

		}
		
		if (mouseLeft && rapidFire)
		{
			rapidCount++;
			if (rapidCount > 1)
			{
				int Action = IInput::FLAG_PRESS;
				if (rapidState) Action = IInput::FLAG_RELEASE;
			
			
				m_aInputCount[m_InputCurrent][KEY_MOUSE_1].m_Presses++;
				if(rapidState)
					m_aInputState[m_InputCurrent][KEY_MOUSE_1] = 1;
				AddEvent(0, KEY_MOUSE_1, Action);
			
				rapidState = !rapidState;
				rapidCount = 0;
			}
		
		}
		
		
	}

	return 0;
}


IEngineInput *CreateEngineInput() { return new CInput; }
