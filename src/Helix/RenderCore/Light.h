#ifndef LIGHT_H
#define LIGHT_H

namespace Helix {

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

const int					MAX_LIGHTS = 100;
Light						m_lights[2][MAX_LIGHTS];

void	ResetFrame();
void	AddPointLight(const D3DXVECTOR3 &position, const D3DXCOLOR &color, const float innerRadius, const float outerRadius);
} // namespace Helix

#endif LIGHT_H