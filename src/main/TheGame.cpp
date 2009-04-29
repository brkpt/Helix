// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "TheGame.h"
#include "RenderCore/DeclManager.h"
#include "RenderCore/ShaderManager.h"
#include "RenderCore/MaterialManager.h"
#include "RenderCore/InstanceManager.h"
#include "RenderCore/RenderThread.h"
#include "Camera.h"
#include "triangle.h"
#include "grid.h"

// ****************************************************************************
// ****************************************************************************
TheGame *	TheGame::m_instance = NULL;

// ****************************************************************************
// ****************************************************************************
TheGame::TheGame(void) :
m_camera(NULL)
{
	memset(&m_mouseState,0,sizeof(m_mouseState));
	memset(m_keyboardState,0,sizeof(m_keyboardState));
}

// ****************************************************************************
// ****************************************************************************
TheGame * TheGame::CreateInstance(void)
{
	_ASSERT(m_instance == NULL);
	m_instance = new TheGame();
	return m_instance;
}

// ****************************************************************************
// ****************************************************************************
bool TheGame::Initialize(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow,bool fullScreen,int width,int height,LPTSTR windowName)
{
	bool retVal = WinApp::Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow, fullScreen, width, height, windowName);

	DeclManager::GetInstance();
	ShaderManager::GetInstance();
	MaterialManager::GetInstance();
	InstanceManager::GetInstance();
	return retVal;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::LoadScene(void)
{
	m_camera = new Camera;

	m_camera->SetPosition(D3DXVECTOR3(0.0f, 1.0f, -4.0f));
	m_camera->SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_camera->SetFocalPoint(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	float aspectRatio = (float)m_windowWidth/(float)m_windowHeight;
	m_camera->BuildProjectionMatrix(D3DX_PI/4.0f,aspectRatio,1.0f,200.0f);


	m_triangle = new Triangle;
	m_grid = new Grid;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::UnloadScene(void)
{
	delete m_triangle;
	delete m_grid;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessMouseMove(LPARAM lParam, WPARAM wParam)
{
	unsigned mouseX,mouseY;
	mouseX = LOWORD(lParam);
	mouseY = HIWORD(lParam);

	m_mouseState.mouseDeltaX = mouseX - m_mouseState.mousePosX;
	m_mouseState.mouseDeltaY = mouseY - m_mouseState.mousePosY;

	//char buffer[1024];
	//sprintf(buffer,"X=%d Y=%d dx=%d dy=%d X'=%d Y'=%d\n",m_mouseState.mousePosX,m_mouseState.mousePosY,
	//	m_mouseState.mouseDeltaX,m_mouseState.mouseDeltaY,
	//	mouseX,mouseY);
	//OutputDebugString(buffer);

	m_mouseState.mousePosX = mouseX;
	m_mouseState.mousePosY = mouseY;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessMouseLBDown(LPARAM lParam, WPARAM wParam)
{
	m_mouseState.leftButtonDown = true;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessMouseLBUp(LPARAM lParam, WPARAM wParam)
{
	m_mouseState.leftButtonDown = false;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessMouseRBDown(LPARAM lParam, WPARAM wParam)
{
	m_mouseState.rightButtonDown = true;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessMouseRBUp(LPARAM lParam, WPARAM wParam)
{
	m_mouseState.rightButtonDown = false;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessMouseMBDown(LPARAM lParam, WPARAM wParam)
{
	m_mouseState.middleButtonDown = true;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessMouseMBUp(LPARAM lParam, WPARAM wParam)
{
	m_mouseState.middleButtonDown = false;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessKeyDown(LPARAM lParam, WPARAM wParam)
{
	m_keyboardState[wParam] = true;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::ProcessKeyUp(LPARAM lParam, WPARAM wParam)
{
	m_keyboardState[wParam] = false;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::Update(void)
{
	WinApp::Update();

	POINT ptCursor;
	GetCursorPos( &ptCursor );
	m_pD3DDevice->SetCursorPosition(ptCursor.x,ptCursor.y,0);

	m_camera->Update();
	m_triangle->Update(0.0f);
	m_grid->Update(0.0f);

	m_mouseState.mouseDeltaX = 0;
	m_mouseState.mouseDeltaY = 0;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::Render(void)
{
	WinApp::Render();

	// Build our view matrix from our camera matrix
	if(m_pD3DDevice == NULL)
	{
		return;
	}

	m_pD3DDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0xff000000, 1.0f, 0L );
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->BeginScene();

	TheGame *game = TheGame::Instance();
	D3DXMATRIX &viewMatrix = game->CurrentCamera()->GetViewMatrix();
	D3DXMATRIX &projMatrix = game->CurrentCamera()->GetProjectionMatrix();

	ShaderManager::GetInstance().SetSharedParameter("WorldView",viewMatrix);
	ShaderManager::GetInstance().SetSharedParameter("Projection",projMatrix);

	Render::RenderScene();

	m_grid->Render();
	m_triangle->Render();
	m_pD3DDevice->EndScene();
	m_pD3DDevice->Present(NULL,NULL,NULL,NULL);
}

