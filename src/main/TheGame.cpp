// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "TheGame.h"
#include "RenderCore/DeclManager.h"
#include "RenderCore/ShaderManager.h"
#include "RenderCore/MaterialManager.h"
#include "RenderCore/InstanceManager.h"
#include "RenderCore/RenderThread.h"
#include "RenderCore/RenderMgr.h"
#include "Kernel/Callback.h"
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

	ID3D10Device *dev = Helix::RenderMgr::GetInstance().GetDevice();
	IDXGISwapChain *sc = Helix::RenderMgr::GetInstance().GetSwapChain();

	Helix::SetDevice(dev,sc);
	Helix::DeclManager::Create();
	Helix::ShaderManager::Create();
	Helix::MaterialManager::Create();
	Helix::MeshManager::Create();
	Helix::InstanceManager::GetInstance();

	return retVal;
}

//class Foo
//{
//public:
//	void MyCallback()
//	{
//		int a = 1;
//	}
//
//	void MyCallback1(int foo)
//	{
//		int a = 1;
//	}
//
//	void MyCallback2(int foo1, int foo2)
//	{
//		int a = 1;
//	}
//
//	static void MyCallbackS0()
//	{
//		int a = 1;
//	}
//
//	static void MyCallbackS1(int foo)
//	{
//		int a = 1;
//	}
//
//	static void MyCallbackS2(int foo, char *whee)
//	{
//		int a = foo;
//		char *test = whee;
//	}
//};

// ****************************************************************************
// ****************************************************************************
void TheGame::LoadScene(void)
{
	//Foo test;

	//Helix::Callback *cb0 = new Helix::MemberCallback0<Foo>(test, &Foo::MyCallback);
	//Helix::Callback *cb1 = new Helix::MemberCallback1<Foo,int>(test,&Foo::MyCallback1,5);
	//Helix::Callback *cb2 = new Helix::MemberCallback2<Foo,int,int>(test,&Foo::MyCallback2,10,20);
	//Helix::Callback *scb0 = new Helix::StaticCallback0(&Foo::MyCallbackS0);
	//Helix::Callback *scb1 = new Helix::StaticCallback1<int>(&Foo::MyCallbackS1,10);
	//Helix::Callback *scb2 = new Helix::StaticCallback2<int, char *>(&Foo::MyCallbackS2,10,"Hello");

	//cb0->Call();
	//cb1->Call();
	//cb2->Call();

	//scb0->Call();
	//scb1->Call();
	//scb2->Call();

	Helix::ShaderManager::GetInstance().LoadShared();
	m_camera = new Camera;

	m_camera->SetPosition(D3DXVECTOR3(0.0f, 1.0f, -4.0f));
	m_camera->SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_camera->SetFocalPoint(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	float aspectRatio = (float)m_windowWidth/(float)m_windowHeight;
	m_camera->BuildProjectionMatrix((float)D3DX_PI/4.0f,aspectRatio,1.0f,200.0f);

	D3DXVECTOR3 sunDir(0.0f, 1.0f, 1.0f);
	Helix::SetSunlightDir( sunDir );

	DXGI_RGB tempColor = { 1.0f,0.25f, 0.25f};
	Helix::SetSunlightColor( tempColor );

	tempColor.Red = 0.25f;
	tempColor.Green = 0.25f;
	tempColor.Blue = 0.25f;
	Helix::SetAmbientColor( tempColor );

	m_world = new Helix::Instance;
	m_world->SetMeshName("World");
	Helix::Mesh *mesh = Helix::MeshManager::GetInstance().Load("holodeck");

}

// ****************************************************************************
// ****************************************************************************
void TheGame::UnloadScene(void)
{
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
//	m_pD3DDevice->SetCursorPosition(ptCursor.x,ptCursor.y,0);

	Helix::SubmitInstance(*m_world);

	m_camera->Update();

	m_mouseState.mouseDeltaX = 0;
	m_mouseState.mouseDeltaY = 0;
}

// ****************************************************************************
// ****************************************************************************
void TheGame::Render(void)
{
	Helix::RenderThreadReady();

	WinApp::Render();

	// Build our view matrix from our camera matrix
	if(m_pD3DDevice == NULL)
	{
		return;
	}

	// Set our camera information
	TheGame *game = TheGame::Instance();
	Helix::SubmitViewMatrix(game->CurrentCamera()->GetViewMatrix());
	Helix::SubmitProjMatrix(game->CurrentCamera()->GetProjectionMatrix());

	Helix::RenderScene();
}

