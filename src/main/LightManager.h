#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H
#include "Utility/Timer.h"

class LightManager
{
public:
	static void Create()
	{
		GetInstance();
	}

	static LightManager & GetInstance()
	{
		static LightManager	instance;
		return instance;
	}

	~LightManager();

	void	Update();
	void	SubmitLights();

private:
	LightManager();
	LightManager(const LightManager &other) {}
	LightManager & operator=(const LightManager &other) {}

	void	UpdateLights();
	void	KillDeadLights();
	void	CreateNewLights();

	static const int NUM_LIGHTS = 100;
	int				m_numLights;
	D3DXVECTOR3		m_lightPositions[NUM_LIGHTS];
	D3DXVECTOR3		m_lightVel[NUM_LIGHTS];
	D3DXCOLOR		m_lightColors[NUM_LIGHTS];
	bool			m_lightKill[NUM_LIGHTS];

	Helix::Timer	m_timer;
	float			m_frameTime;
	float			m_lightBatchTime;
	bool			m_firstFrame;

};
#endif // LIGHTMANAGER_H
