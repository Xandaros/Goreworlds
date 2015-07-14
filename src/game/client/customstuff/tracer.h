#ifndef GAME_CLIENT_TRACER_H
#define GAME_CLIENT_TRACER_H
#include <base/vmath.h>

#define MAX_TRACES 20


class CTracer
{
	struct CTrace
	{
		vec2 m_P1, m_P2;
		vec4 m_C1, m_C2;
	};
	
	CTrace m_aTrace[MAX_TRACES];
	
	bool m_TracerOn;
	
	int m_CurrentTrace;
	float m_FadeSpeed;
	float m_Life;
	vec4 m_Color1, m_Color2;

	vec2 m_TraceP1, m_TraceP2;
	
	int ToNext()
	{
		m_CurrentTrace++;
		if (m_CurrentTrace >= MAX_TRACES)
			m_CurrentTrace -= MAX_TRACES;

		return m_CurrentTrace;
	}
	
	int Next()
	{
		if (m_CurrentTrace + 1 >= MAX_TRACES)
			return m_CurrentTrace + 1 - MAX_TRACES;
		else
			return m_CurrentTrace + 1;
	}
	
	int Next(int i)
	{
		if (i + 1 >= MAX_TRACES)
			return i + 1 - MAX_TRACES;
		else
			return i + 1;
	}	
	
	int Previous(int i)
	{
		if (i - 1 < 0)
			return i - 1 + MAX_TRACES;
		else
			return i - 1;
	}
	
public:
	CTracer()
	{
		Clean();
		SetColor(vec4(0, 0, 0, 0));
	}
	
	~CTracer(){}
	
	void Clean();
	void Render(class CRenderTools *pRenderTools);
	void Update(bool AddTracer = false);
	
	void SetTracePos(vec2 TracerStart, vec2 TracerEnd);
	
	void SetColor(vec4 Color)
	{
		m_Color1 = Color;
		m_Color2 = Color;
	}
};





#endif