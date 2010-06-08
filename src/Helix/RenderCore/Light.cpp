#include "stdafx.h"
#include "Light.h"
#include "RenderThread.h"

namespace Helix {

int		m_lightIndex = 0;

void ResetFrame()
{
	m_lightIndex = 0;
}

void	AddPointLight(const D3DXVECTOR3& position, const D3DXCOLOR &color, const float innerRadius, const float outerRadius)
{
	int submissionIndex = SubmissionIndex();
	Light &light = m_lights[SubmissionIndex()][m_lightIndex];

	light.m_type = Light::POINT;
	light.m_color = color;

	// Bit of a hack here
	// Treat the underlying storage as a D3DXVECTOR3 type and use the assignment operator
	// to copy values over
	*((D3DXVECTOR3 *)&light.point.m_position) = position;

	light.point.m_innerRadius = innerRadius;
	light.point.m_outerRadius = outerRadius;
}

}