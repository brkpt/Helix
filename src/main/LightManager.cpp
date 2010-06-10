#include "stdafx.h"
#include <time.h>
#include "LightManager.h"
#include "RenderCore/Light.h"

// ****************************************************************************
// ****************************************************************************
LightManager::LightManager()
: m_numLights(0)
, m_firstFrame(true)
{
	for(int index=0;index<NUM_LIGHTS;index++)
	{
		m_lightPositions[index].x=0;
		m_lightPositions[index].y=0;
		m_lightPositions[index].z=0;

		m_lightColors[index].r=0;
		m_lightColors[index].g=0;
		m_lightColors[index].b=0;

		m_lightVel[index].x=0;
		m_lightVel[index].y=0;
		m_lightVel[index].z=0;

		m_lightKill[index] = false;
	}
}
// ****************************************************************************
// ****************************************************************************
LightManager::~LightManager()
{}

// ****************************************************************************
// ****************************************************************************
void LightManager::UpdateLights()
{
	for(int lightIndex=0;lightIndex < m_numLights; lightIndex++)
	{
		D3DXVECTOR3 accel(0.0f, -9.8f, 0.0f);
		D3DXVECTOR3 deltaVel = accel*m_frameTime;
		m_lightVel[lightIndex] += deltaVel;

		D3DXVECTOR3 deltaPos = m_lightVel[lightIndex]*m_frameTime;
		m_lightPositions[lightIndex] = m_lightPositions[lightIndex] + deltaPos;
	}
}

// ****************************************************************************
// ****************************************************************************
void LightManager::KillDeadLights()
{
	for(int lightIndex=0;lightIndex < m_numLights; lightIndex++)
	{
		if(m_lightPositions[lightIndex].y < -20.0f)
		{
			m_lightKill[lightIndex] = true;
		}
	}

	for(int lightIndex=0; lightIndex < m_numLights; lightIndex++)
	{
		if(m_lightKill[lightIndex])
		{
			if(lightIndex < m_numLights-1)
			{
				int count = m_numLights - lightIndex - 1;
				memmove(&m_lightPositions[lightIndex], &m_lightPositions[lightIndex+1], count*sizeof(D3DXVECTOR3));
				memmove(&m_lightVel[lightIndex], &m_lightVel[lightIndex+1], count*sizeof(D3DXVECTOR3));
				memmove(&m_lightColors[lightIndex], &m_lightColors[lightIndex+1], count*sizeof(D3DXCOLOR));
				memmove(&m_lightKill[lightIndex], &m_lightKill[lightIndex+1], count*sizeof(bool));
			}
			
			// Backup one so we properly check the next light
			lightIndex = lightIndex-1;
			m_numLights--;

			// Clean up the last entry
			m_lightPositions[m_numLights].x=0;
			m_lightPositions[m_numLights].y=0;
			m_lightPositions[m_numLights].z=0;

			m_lightColors[m_numLights].r=0;
			m_lightColors[m_numLights].g=0;
			m_lightColors[m_numLights].b=0;

			m_lightVel[m_numLights].x=0;
			m_lightVel[m_numLights].y=0;
			m_lightVel[m_numLights].z=0;

			m_lightKill[m_numLights] = false;
		}

	}
}

// ****************************************************************************
// ****************************************************************************
void LightManager::CreateNewLights()
{
	if(m_lightBatchTime > 100.0f)
	{
		// Create 10 lights
		for(int i=0;i<10 && m_numLights < NUM_LIGHTS;i++)
		{
			D3DXVECTOR3 &pos = m_lightPositions[m_numLights];
			pos.x = 20.0f * static_cast<float>(rand())/32767.0f - 10.0f;
			pos.y = 10.0f + 3.0f * static_cast<float>(rand())/32767.0;
			pos.z = 20.0f * static_cast<float>(rand())/32767.0f - 10.0f;
			
			D3DXCOLOR &color = m_lightColors[m_numLights];
			color.r = static_cast<float>(rand())/32767.0f;
			color.g = static_cast<float>(rand())/32767.0f;
			color.b = static_cast<float>(rand())/32767.0f;

			m_numLights++;
		}

		m_lightBatchTime = 0.0f;
	}
}

// ****************************************************************************
// ****************************************************************************
void LightManager::SubmitLights()
{
	for(int iLightIndex=0;iLightIndex<m_numLights;iLightIndex++)
	{
		D3DXVECTOR3 &pos = m_lightPositions[iLightIndex];
		D3DXCOLOR &color = m_lightColors[iLightIndex];
		Helix::AddPointLight(pos,color,1.0f,5.0f);
	}
}
// ****************************************************************************
// ****************************************************************************
void LightManager::Update()
{
	m_frameTime = m_timer.LapSeconds();
	m_timer.LapMark();

	m_lightBatchTime += m_frameTime * 1000.0f;	// in milliseconds

	if(m_firstFrame)
	{
		m_timer.Start();
		m_firstFrame = false;
		m_lightBatchTime = 1000.0f;		// Forces light creation on first frame
		m_frameTime = 0.0f;
	}

	UpdateLights();
	KillDeadLights();
	CreateNewLights();
}

