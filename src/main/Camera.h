#ifndef CAMERA_DOT_H
#define CAMERA_DOT_H

#include "Math/Vector.h"
#include "Math/Matrix.h"

#define	PIXELS_TO_RADIANS	(Helix::PI/384.0f)
#define	PIXELS_TO_DISTANCE	(1.0f/50.0f)			// 1m per 50 pixels

class Camera
{
public:
	Camera(void);

	void	Update(void);
	void	SetPosition(const Helix::Vector3 &pos)
	{
		m_worldMatrix.r[3][0] = pos.x;
		m_worldMatrix.r[3][1] = pos.y;
		m_worldMatrix.r[3][2] = pos.z;
	}

	void	SetUp(const Helix::Vector3 &up)
	{
		m_up = up;
	}

	void	SetDir(const Helix::Vector3 &forw);
	void	SetFocalPoint(const Helix::Vector3 &focal)
	{
		m_focalPoint = focal;
	}

	void	Reset()
	{
		m_worldMatrix.SetIdentity();
	}

	void	BuildProjectionMatrix(float fovY, float aspect, float near, float far);
	void	MoveForwardCameraRelative(const float &dist);
	void	Strafe(const float &dist);
	void	Pan(const float & radians);
	void	Dolly(const Helix::Vector3 & translation);
	void	Tilt(const float & radians);
	Helix::Matrix4x4 &	GetWorldMatrix(void) 		{ return m_worldMatrix; }
	Helix::Matrix4x4 &	GetViewMatrix(void)			{ return m_viewMatrix; }
	Helix::Matrix4x4 &	GetProjectionMatrix(void)	{ return m_projMatrix; }

protected:
	void	BuildMatrices(void);

	Helix::Vector3		m_position;
	Helix::Vector3		m_up;
	Helix::Vector3		m_focalPoint;

	Helix::Matrix4x4	m_worldMatrix;
	Helix::Matrix4x4	m_viewMatrix;
	Helix::Matrix4x4	m_projMatrix;
};

#endif // CAMERA_DOT_H