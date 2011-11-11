// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "Win32DX10.h"
#include "RenderCore/RenderThread.h"
#include "RenderCore/RenderMgr.h"
#include "ThreadLoad/ThreadLoad.h"

// ****************************************************************************
// ****************************************************************************
WinApp *	WinApp::m_instance = NULL;

// ****************************************************************************
// ****************************************************************************
WinApp::WinApp(void) :
m_pD3DDevice(NULL), 
m_fullScreen(false),
m_hDC(NULL),
m_appInstance(NULL)
{
	_ASSERT(m_instance == NULL);
	m_instance = this;

}

WinApp::~WinApp(void)
{
}

// ****************************************************************************
// ****************************************************************************
bool WinApp::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow, bool fullScreen, int width, int height, LPTSTR windowName)
{
	if(!InitializeWin32(hInstance, hPrevInstance, lpCmdLine, nCmdShow, fullScreen, width, height, windowName))
		return false;

	if(!InitializeDirectX())
		return false;

	ShowWindow(m_hWnd,SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	return true;
}

// ****************************************************************************
// ****************************************************************************
void WinApp::Cleanup(void)
{
	CleanupDirectX();
	CleanupWin32();
}

// ****************************************************************************
// ****************************************************************************
bool WinApp::InitializeDirectX(void)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	memset(&swapChainDesc,0,sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = m_windowWidth;
	swapChainDesc.BufferDesc.Height = m_windowHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	IDXGISwapChain *swapChain;
	ID3D11DeviceContext *context;
	HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, NULL, 0,
											D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &m_pD3DDevice, NULL, &context );

	_ASSERT( SUCCEEDED(hr) );

	Helix::RenderMgr::GetInstance().SetDXDevice(m_pD3DDevice, context, swapChain);
	return true;
}

// ****************************************************************************
// ****************************************************************************
void WinApp::CleanupDirectX(void)
{
	// Destroy D3D device
	if(m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
}

// ****************************************************************************
// ****************************************************************************
bool WinApp::InitializeWin32(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow, bool fullScreen, unsigned short width, unsigned short height, LPTSTR windowTitle)
{
	m_appInstance = hInstance;

	RECT	windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = width;
	windowRect.bottom = height;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_CLASSDC;
	wcex.lpfnWndProc	= (WNDPROC)WinApp::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "D3D";
	wcex.hIconSm		= NULL;

	if(!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;	
	}

	DWORD	userExStyle=0;
	DWORD	userStyle=0;

	if(fullScreen)
	{
		userExStyle = WS_EX_APPWINDOW;
		userStyle = WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		userExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		userStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&windowRect,userStyle,FALSE,userExStyle);

	m_hWnd = CreateWindowEx(userExStyle,"D3D", 
		windowTitle, 
		userStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 
		0, 
		windowRect.right, windowRect.bottom, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	if(m_hWnd == NULL)
	{
		CleanupWin32();
		MessageBox(NULL,"Couldn't create window","D3D Window",MB_OK);
		return false;
	}

	m_hDC = GetDC(m_hWnd);
	if(m_hDC == NULL)
	{
		CleanupWin32();
		MessageBox(NULL,"Couldn't get DC","D3D Window",MB_OK);
	}

	m_fullScreen = fullScreen;
	m_windowWidth = width;
	m_windowHeight = height;
	m_hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WIN32DX9);
	return true;
}

// ****************************************************************************
// ****************************************************************************
void WinApp::CleanupWin32(void)
{
	// Restore display settings
	if(m_fullScreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}

	// Release DC
	if(m_hDC != NULL)
	{
		if(!ReleaseDC(m_hWnd,m_hDC))
		{
			MessageBox(NULL,"Could not release DC!","D3D Window",MB_OK);
			m_hDC = NULL;
		}
	}

	// Destroy our window
	if(m_hWnd)
	{
		if(!DestroyWindow(m_hWnd))
		{
			MessageBox(NULL,"Could not destroy window!","D3D Window",MB_OK);
			m_hWnd = NULL;
		}
	}

	// Unregister our window class
	if (!UnregisterClass("D3D",m_appInstance))// Able To Unregister Class?
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_appInstance=NULL;
	}
}

// ****************************************************************************
// ****************************************************************************
void WinApp::Update(void)
{
}

// ****************************************************************************
// ****************************************************************************
void WinApp::Render(void)
{
}

// ****************************************************************************
// ****************************************************************************
int WinApp::Run(void)
{
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );
	
	// Main message loop:
	while( msg.message!=WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			Update();
			Render();
		}
	}

	return (int) msg.wParam;
}

// ****************************************************************************
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
// ****************************************************************************
LRESULT CALLBACK WinApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL leftMouse = false;
	switch( message )
	{
	case WM_ACTIVATE:
		{
			if(!HIWORD(wParam))
			{
				// Program is active
			}
			else
			{
				// Program is minimized
			}
			return 0;
		}
	
	case WM_SYSCOMMAND:
		{
			switch(wParam)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;	// do not kick in
				break;
			}
			break;
		}

	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

	case WM_MOUSEMOVE:
		{
			WinApp::Instance()->ProcessMouseMove(lParam, wParam);
			return 0;
		}

	case WM_LBUTTONDOWN:
		{
			WinApp::Instance()->ProcessMouseLBDown(lParam,wParam);
			return 0;
		}

	case WM_LBUTTONUP:
		{
			WinApp::Instance()->ProcessMouseLBUp(lParam, wParam);
			return 0;
		}

	case WM_RBUTTONDOWN:
		{
			WinApp::Instance()->ProcessMouseRBDown(lParam, wParam);
			return 0;
		}

	case WM_RBUTTONUP:
		{
			WinApp::Instance()->ProcessMouseRBUp(lParam, wParam);
			return 0;
		}

	case WM_MBUTTONDOWN:
		{
			WinApp::Instance()->ProcessMouseMBDown(lParam, wParam);
			return 0;
		}

	case WM_MBUTTONUP:
		{
			WinApp::Instance()->ProcessMouseMBUp(lParam, wParam);
			return 0;
		}

	case WM_KEYDOWN:
		{
			WinApp::Instance()->ProcessKeyDown(lParam, wParam);
			return 0;
		}
	case WM_KEYUP:
		{
			WinApp::Instance()->ProcessKeyUp(lParam, wParam);
			return 0;
		}

	case WM_CHAR:
		{
			break;
		}

//	case WM_PAINT:
//		PAINTSTRUCT ps;
//		BeginPaint(hWnd,&ps);
////		WinApp::GetInstance()->Render();
//		EndPaint(hWnd,&ps);
//		break;

	case WM_SETCURSOR:
		{
			break;
		}

	case WM_SIZE:
		{
			// LOWORD(lParam) = width
			// LOWORD(lParam) = height
			return 0;
		}

	case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

