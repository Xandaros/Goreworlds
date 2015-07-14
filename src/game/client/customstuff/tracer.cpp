#include "tracer.h"

#include <math.h>

#include <base/math.h>

#include <engine/graphics.h>
#include <game/generated/client_data.h>
#include <game/generated/protocol.h>

#include <game/client/render.h>


void CTracer::Clean()
{
	for (int i = 0; i < MAX_TRACES; i++)
	{
		m_aTrace[i].m_C1.a = 0.0f;
		m_aTrace[i].m_C2.a = 0.0f;
	}
}


void CTracer::Render(CRenderTools *pRenderTools)
{
	pRenderTools->Graphics()->BlendNormal();
	pRenderTools->Graphics()->TextureSet(-1);
	pRenderTools->Graphics()->QuadsBegin();
	
	int Trace = Next();
	
	// render from Next() to m_CurrentTrace
	
	
	for (int i = 0; i < MAX_TRACES; i++)
	{
		int Prev = Previous(Trace);
		
		if (i != 0)
		if ((m_aTrace[Trace].m_C1.a > 0.0f || m_aTrace[Trace].m_C2.a > 0.0f) &&
			(m_aTrace[Prev].m_C1.a > 0.0f || m_aTrace[Prev].m_C2.a > 0.0f))
		{
			IGraphics::CColorVertex aColors[4] = {
				IGraphics::CColorVertex(0, m_aTrace[Prev].m_C2.r, m_aTrace[Prev].m_C2.g, m_aTrace[Prev].m_C2.b, m_aTrace[Prev].m_C2.a),
				IGraphics::CColorVertex(1, m_aTrace[Trace].m_C2.r, m_aTrace[Trace].m_C2.g, m_aTrace[Trace].m_C2.b, m_aTrace[Trace].m_C2.a),
				IGraphics::CColorVertex(2, m_aTrace[Prev].m_C1.r, m_aTrace[Prev].m_C1.g, m_aTrace[Prev].m_C1.b, m_aTrace[Prev].m_C1.a),
				IGraphics::CColorVertex(3, m_aTrace[Trace].m_C1.r, m_aTrace[Trace].m_C1.g, m_aTrace[Trace].m_C1.b, m_aTrace[Trace].m_C1.a)};
			pRenderTools->Graphics()->SetColorVertex(aColors, 4);
			
			IGraphics::CFreeformItem Freeform(
					m_aTrace[Prev].m_P2.x, m_aTrace[Prev].m_P2.y,
					m_aTrace[Trace].m_P2.x, m_aTrace[Trace].m_P2.y,
					m_aTrace[Prev].m_P1.x, m_aTrace[Prev].m_P1.y,
					m_aTrace[Trace].m_P1.x, m_aTrace[Trace].m_P1.y);
							
			pRenderTools->Graphics()->QuadsDrawFreeform(&Freeform, 1);
		}
		
		Trace = Next(Trace);
	}
	
	
	Trace = m_CurrentTrace;
	
	// draw from m_CurrentTrace to Sword
	/*
	if ((m_aTrace[Trace].m_C1.a > 0.0f || m_aTrace[Trace].m_C2.a > 0.0f) && m_TracerOn)
	{
			IGraphics::CColorVertex aColors[4] = {
				IGraphics::CColorVertex(0, m_aTrace[Trace].m_C2.r, m_aTrace[Trace].m_C2.g, m_aTrace[Trace].m_C2.b, m_aTrace[Trace].m_C2.a),
				IGraphics::CColorVertex(1, m_Color2.r, m_Color2.g, m_Color2.b, m_Color2.a),
				IGraphics::CColorVertex(2, m_aTrace[Trace].m_C1.r, m_aTrace[Trace].m_C1.g, m_aTrace[Trace].m_C1.b, m_aTrace[Trace].m_C1.a),
				IGraphics::CColorVertex(3, m_Color1.r, m_Color1.g, m_Color1.b, m_Color1.a)};
			pRenderTools->Graphics()->SetColorVertex(aColors, 4);
							
			IGraphics::CFreeformItem Freeform(
					m_aTrace[Trace].m_P2.x, m_aTrace[Trace].m_P2.y,
					m_TraceP2.x, m_TraceP2.y,
					m_aTrace[Trace].m_P1.x, m_aTrace[Trace].m_P1.y,
					m_TraceP1.x, m_TraceP1.y);
							
			pRenderTools->Graphics()->QuadsDrawFreeform(&Freeform, 1);
	}
	*/
	
	
	pRenderTools->Graphics()->QuadsEnd();
}

void CTracer::SetTracePos(vec2 TracerStart, vec2 TracerEnd)
{
	m_TraceP1 = TracerStart;
	m_TraceP2 = TracerEnd;
}


void CTracer::Update(bool AddTracer)
{
	if (AddTracer)
	{
		m_TracerOn = true;
		int i = ToNext();
		
		m_aTrace[i].m_P1 = m_TraceP1;
		m_aTrace[i].m_P2 = m_TraceP2;
		
		m_aTrace[i].m_C1 = m_Color1;
		m_aTrace[i].m_C2 = m_Color2;
	}
	else
		m_TracerOn = false;
	
	for (int i = 0; i < MAX_TRACES; i++)
	{
		if (m_aTrace[i].m_C1.a > 0.0f)
			m_aTrace[i].m_C1.a -= 0.04f;
		
		if (m_aTrace[i].m_C2.a > 0.0f)
			m_aTrace[i].m_C2.a -= 0.04f;
	}
}



