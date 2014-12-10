#ifndef LIGHT_H
#define LIGHT_H

#include "Math/Color.h"

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
	Helix::Color		m_color;
};

void	InitializeLights();
void	ResetLights();
void	AddPointLight(const Helix::Vector3 &position, const Helix::Color &color, const float innerRadius, const float outerRadius);
bool	AquireLightMutex();
bool	ReleaseLightMutex();
Light *	SubmittedLights();
int		SubmittedLightCount();

} // namespace Helix

#endif LIGHT_H