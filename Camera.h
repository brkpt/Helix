#ifndef CAMERA_DOT_H
#define CAMERA_DOT_H

#include <d3dx9.h>

#define	PIXELS_TO_RADIANS	(D3DX_PI/384.0f)
#define	PIXELS_TO_DISTANCE	(1.0f/10.0f)			// 1m per 5 pixels

class Camera
{
public:
	Camera(void);

	void	Update(void);
	void	SetPosition(const D3DXVECTOR3 &pos)
	{
		m_worldMatrix.m[3][0] = pos.x;
		m_worldMatrix.m[3][1] = pos.y;
		m_worldMatrix.m[3][2] = pos.z;
	}
	void	SetUp(const D3DXVECTOR3 &up)
	{
		m_up = up;
	}
	void	SetFocalPoint(const D3DXVECTOR3 &focal)
	{
		m_focalPoint = focal;
	}

	void	BuildProjectionMatrix(float fovY, float aspect, float near, float far);
	void	MoveForwardCameraRelative(const float &dist);
	void	Strafe(const float &dist);
	void	Pan(const float & radians);
	void	Dolly(const D3DXVECTOR3 & translation);
	void	Tilt(const float & radians);
	D3DXMATRIX &	GetWorldMatrix(void) 		{ return m_worldMatrix; }
	D3DXMATRIX &	GetViewMatrix(void)			{ return m_viewMatrix; }
	D3DXMATRIX &	GetProjectionMatrix(void)	{ return m_projMatrix; }

protected:
	void	BuildMatrices(void);

	D3DXVECTOR3		m_position;
	D3DXVECTOR3		m_up;
	D3DXVECTOR3		m_focalPoint;

	D3DXMATRIX		m_worldMatrix;
	D3DXMATRIX		m_viewMatrix;
	D3DXMATRIX		m_projMatrix;
};

#endif // CAMERA_DOT_H