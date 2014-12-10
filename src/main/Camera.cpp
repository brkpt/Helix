#include "Math/MathDefs.h"
#include "Camera.h"
#include "TheGame.h"

// ****************************************************************************
// ****************************************************************************
Camera::Camera(void)
{
	m_worldMatrix.SetIdentity();
	m_position = Helix::Vector3(0.0f,0.0f,0.0f);
	m_up = Helix::Vector3(0.0f,1.0f,0.0f);
	m_focalPoint = Helix::Vector3(0.0f, 0.0f, 1.0f);
}

// ****************************************************************************
// ****************************************************************************
void Camera::MoveForwardCameraRelative(const float &dist)
{
	Helix::Vector3 cameraDir;
	cameraDir.x = m_worldMatrix.r[2][0];
	cameraDir.y = m_worldMatrix.r[2][1];
	cameraDir.z = m_worldMatrix.r[2][2];

	Helix::Matrix4x4 t;
	t.SetTranslation(cameraDir.x*dist, cameraDir.y*dist, cameraDir.z*dist);
	m_worldMatrix = m_worldMatrix * t;
}
// ****************************************************************************
// ****************************************************************************
void Camera::Update(void)
{
	TheGame *game = TheGame::Instance();
	MouseState mouseState = TheGame::Instance()->MouseState();

	if(mouseState.rightButtonDown && mouseState.mouseDeltaY != 0)
	{
		MoveForwardCameraRelative(mouseState.mouseDeltaY * PIXELS_TO_DISTANCE);
	}

	if(mouseState.middleButtonDown)
	{
		if(mouseState.mouseDeltaY != 0)
		{
			Helix::Vector3 t(0.0f, -1.0f, 0.0f);
			t.y = mouseState.mouseDeltaY * PIXELS_TO_DISTANCE;
			Dolly(t);
		}
		if(mouseState.mouseDeltaX != 0)
		{
			Helix::Vector3 t(m_worldMatrix.r[0][0], m_worldMatrix.r[0][1], m_worldMatrix.r[0][2]);
			t = t * mouseState.mouseDeltaX * PIXELS_TO_DISTANCE;
			Dolly(t);
		}
	}

	if(mouseState.leftButtonDown)
	{
		if(mouseState.mouseDeltaX != 0)
		{
			Pan((float)mouseState.mouseDeltaX * PIXELS_TO_RADIANS);
		}
		if(mouseState.mouseDeltaY != 0)
		{
			Tilt((float)mouseState.mouseDeltaY * PIXELS_TO_RADIANS);
		}
	}

	if(game->KeyDown('a') || game->KeyDown('A'))
	{
		MoveForwardCameraRelative(0.03f);
	}

	if(game->KeyDown('s') || game->KeyDown('S'))
	{
		MoveForwardCameraRelative(-0.03f);
	}

	if(game->KeyDown('w') || game->KeyDown('W'))
	{
		Dolly(Helix::Vector3(0.0f, 0.03f, 0.0f));
	}

	if(game->KeyDown('x') || game->KeyDown('X'))
	{
		Dolly(Helix::Vector3(0.0f, -0.03f, 0.0f));
	}

	if(game->KeyDown('r') || game->KeyDown('R'))
	{
		Reset();
	}
	BuildMatrices();
}

// ****************************************************************************
// ****************************************************************************
void Camera::Pan(const float &radians)
{
	// Translate back to origin, rotate around Y, translate back
	Helix::Matrix4x4 t,tr;
	t.SetTranslation(m_worldMatrix.r[3][0],m_worldMatrix.r[3][1],m_worldMatrix.r[3][2]);
	tr.SetTranslation(-m_worldMatrix.r[3][0],-m_worldMatrix.r[3][1],-m_worldMatrix.r[3][2]);

	Helix::Matrix4x4 r;
	r.SetYRotation(radians);

	Helix::Matrix4x4 temp = m_worldMatrix*tr;
	temp = temp * r;
	m_worldMatrix = temp * t;
}

// ****************************************************************************
// ****************************************************************************
void Camera::Dolly(const Helix::Vector3 & translation)
{
	Helix::Matrix4x4 m;
	m.SetTranslation(translation.x,translation.y,translation.z);
	m_worldMatrix = m_worldMatrix * m;
}

// ****************************************************************************
// ****************************************************************************
void Camera::Tilt(const float &radians)
{
	// Rotate around X, then transform back
	Helix::Matrix4x4 m;

	m.SetXRotation(radians);
	m_worldMatrix = m * m_worldMatrix;
}
// ****************************************************************************
// ****************************************************************************
void Camera::BuildMatrices(void)
{
	//D3DXMatrixIdentity(&m_worldMatrix);

	//D3DXVECTOR3	forward,right,up;
	//forward = m_focalPoint - m_position;
	//D3DXVec3Normalize(&forward,&forward);
	//D3DXVec3Cross(&right,&m_up,&forward);
	//D3DXVec3Normalize(&right,&right);
	//D3DXVec3Cross(&up,&forward,&right);
	//D3DXVec3Normalize(&up,&up);

	//m_worldMatrix.m[0][0] = right.x;
	//m_worldMatrix.m[0][1] = right.y;
	//m_worldMatrix.m[0][2] = right.z;

	//m_worldMatrix.m[1][0] = up.x;
	//m_worldMatrix.m[1][1] = up.y;
	//m_worldMatrix.m[1][2] = up.z;

	//m_worldMatrix.m[2][0] = forward.x;
	//m_worldMatrix.m[2][1] = forward.y;
	//m_worldMatrix.m[2][2] = forward.z;
	//
	//m_worldMatrix.m[3][0] = m_position.x;
	//m_worldMatrix.m[3][1] = m_position.y;
	//m_worldMatrix.m[3][2] = m_position.z;

	m_viewMatrix = m_worldMatrix;
	m_viewMatrix.Invert();
}

// ****************************************************************************
// ****************************************************************************
void Camera::BuildProjectionMatrix(float fovY, float fAspect, float fNear, float fFar)
{
	m_projMatrix.SetProjectionFOV(fovY, fAspect, fNear, fFar);
}

// ****************************************************************************
// ****************************************************************************
void Camera::SetDir(const Helix::Vector3 &forw)
{
	m_worldMatrix.r[2][0] = forw.x;
	m_worldMatrix.r[2][1] = forw.y;
	m_worldMatrix.r[2][2] = forw.z;
	m_worldMatrix.r[2][3] = 0;

	m_worldMatrix.r[1][0] = m_up.x;
	m_worldMatrix.r[1][1] = m_up.y;
	m_worldMatrix.r[1][2] = m_up.z;
	m_worldMatrix.r[1][3] = 0;
}