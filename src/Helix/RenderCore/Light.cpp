#include "stdafx.h"
#include "Light.h"
#include "RenderThread.h"

namespace Helix {

int			m_lightIndex = 0;
HANDLE		m_submitLightMutex;
Light		m_lights[MAX_LIGHTS];

// ****************************************************************************
// ****************************************************************************
void InitializeLights()
{
	m_lightIndex = 0;
	m_submitLightMutex = CreateMutex(NULL,false,"SubmitLight");
}

// ****************************************************************************
// ****************************************************************************
void ShutdownLights()
{
	CloseHandle(m_submitLightMutex);
}

// ****************************************************************************
// ****************************************************************************
void ResetLights()
{
	m_lightIndex = 0;
}

// ****************************************************************************
// ****************************************************************************
int SubmittedLightCount()
{
	return m_lightIndex;
}

// ****************************************************************************
// ****************************************************************************
Light * SubmittedLights()
{
	return m_lights;
}

// ****************************************************************************
// ****************************************************************************
void AddPointLight(const D3DXVECTOR3& position, const D3DXCOLOR &color, const float innerRadius, const float outerRadius)
{
	Light &light = m_lights[m_lightIndex];

	light.m_type = Light::POINT;
	light.m_color = color;

	// Bit of a hack here
	// Treat the underlying storage as a D3DXVECTOR3 type and use the assignment operator
	// to copy values over
	*((D3DXVECTOR3 *)&light.point.m_position) = position;

	light.point.m_innerRadius = innerRadius;
	light.point.m_outerRadius = outerRadius;

	m_lightIndex++;
}

// ****************************************************************************
// ****************************************************************************
bool AquireLightMutex()
{
	DWORD result = WaitForSingleObject(m_submitLightMutex,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	return result == WAIT_OBJECT_0;
}

// ****************************************************************************
// ****************************************************************************
bool ReleaseLightMutex()
{
	DWORD result = ::ReleaseMutex(m_submitLightMutex);
	_ASSERT(result);

	return result != 0;
}
}