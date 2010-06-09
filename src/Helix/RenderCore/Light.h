#ifndef LIGHT_H
#define LIGHT_H

namespace Helix {

const int	MAX_LIGHTS = 100;

struct Light
{
	typedef enum {POINT, DIRECTIONAL, SPOT} LightType;

	LightType	m_type;

	union {
		struct Directional {
			float		m_dir[3];
		} dir;

		struct Cone {
			float		m_position[3];
			float		m_dir[3];
		} cone;

		struct Point {
			float		m_position[3];
			float		m_innerRadius;
			float		m_outerRadius;
		} point;
	};
	D3DXCOLOR		m_color;
};

void	InitializeLights();
void	ResetLights();
void	AddPointLight(const D3DXVECTOR3 &position, const D3DXCOLOR &color, const float innerRadius, const float outerRadius);
bool	AquireLightMutex();
bool	ReleaseLightMutex();
Light *	SubmittedLights();
int		SubmittedLightCount();

} // namespace Helix

#endif LIGHT_H