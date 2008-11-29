#include "stdafx.h"
#include "Camera.h"
#include "TheGame.h"

// ****************************************************************************
// ****************************************************************************
Camera::Camera(void)
{
	D3DXMatrixIdentity(&m_worldMatrix);
	m_position = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_up = D3DXVECTOR3(0.0f,1.0f,0.0f);
	m_focalPoint = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

// ****************************************************************************
// ****************************************************************************
void Camera::MoveForwardCameraRelative(const float &dist)
{
	D3DXVECTOR3 cameraDir;
	cameraDir.x = m_worldMatrix.m[2][0];
	cameraDir.y = m_worldMatrix.m[2][1];
	cameraDir.z = m_worldMatrix.m[2][2];

	D3DXMATRIX t;
	D3DXMatrixTranslation(&t,cameraDir.x*dist,cameraDir.y*dist,cameraDir.z*dist);
	D3DXMatrixMultiply(&m_worldMatrix,&m_worldMatrix,&t);
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
			D3DXVECTOR3 t = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			t.y = mouseState.mouseDeltaY * PIXELS_TO_DISTANCE;
			Dolly(t);
		}
		if(mouseState.mouseDeltaX != 0)
		{
			D3DXVECTOR3 t = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			t.x = mouseState.mouseDeltaX * PIXELS_TO_DISTANCE;
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
		Dolly(D3DXVECTOR3(0.0f, 0.03f, 0.0f));
	}

	if(game->KeyDown('x') || game->KeyDown('X'))
	{
		Dolly(D3DXVECTOR3(0.0f, -0.03f, 0.0f));
	}

	BuildMatrices();
}

// ****************************************************************************
// ****************************************************************************
void Camera::Pan(const float &radians)
{
	// Translate back to origin, rotate around Y, translate back
	D3DXMATRIX t,tr;
	D3DXMatrixTranslation(&t,m_worldMatrix.m[3][0],m_worldMatrix.m[3][1],m_worldMatrix.m[3][2]);
	D3DXMatrixTranslation(&tr,-m_worldMatrix.m[3][0],-m_worldMatrix.m[3][1],-m_worldMatrix.m[3][2]);

	D3DXMATRIX r;
	D3DXMatrixRotationY(&r,radians);

	D3DXMATRIX temp;
	D3DXMatrixMultiply(&temp,&m_worldMatrix,&tr);
	D3DXMatrixMultiply(&temp,&temp,&r);
	D3DXMatrixMultiply(&m_worldMatrix,&temp,&t);
}

// ****************************************************************************
// ****************************************************************************
void Camera::Dolly(const D3DXVECTOR3 & translation)
{
	D3DXMATRIX m;
	D3DXMatrixTranslation(&m,translation.x,translation.y,translation.z);
	D3DXMatrixMultiply(&m_worldMatrix,&m_worldMatrix,&m);
}

// ****************************************************************************
// ****************************************************************************
void Camera::Tilt(const float &radians)
{
	// Rotate around X, then transform back
	D3DXMATRIX m;

	D3DXMatrixRotationX(&m,radians);
	D3DXMatrixMultiply(&m_worldMatrix,&m,&m_worldMatrix);
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

	D3DXMatrixInverse(&m_viewMatrix,NULL,&m_worldMatrix);
}

// ****************************************************************************
// ****************************************************************************
void Camera::BuildProjectionMatrix(float fovY, float fAspect, float fNear, float fFar)
{
	D3DXMatrixPerspectiveFovLH(&m_projMatrix,fovY,fAspect,fNear,fFar);
}