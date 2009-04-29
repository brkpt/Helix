// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "RenderCore/RenderThread.h"
#include "RenderCore/RenderMgr.h"
#include "Helix/ThreadLoad/ThreadLoad.h"

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

	Render::InitializeRenderThread();
	Helix::InitializeThreadLoader();
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
	HRESULT hr;
	DWORD deviceFlags = 0;
	D3DFORMAT backbufferFormat = D3DFMT_UNKNOWN;

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if(m_pD3D == NULL)
	{
		Cleanup();
		MessageBox(NULL,"Could not find D3D version 9","D3D Window",MB_OK);
		return false;
	}

	hr = m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&m_deviceCaps);
	if(FAILED(hr))
	{
		Cleanup();
		return false;
	}

	if( m_deviceCaps.VertexProcessingCaps != 0 )
	{
		deviceFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		deviceFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&m_displayMode);
	if(FAILED(hr))
	{
		Cleanup();
		return false;
	}

	if(m_fullScreen)
	{
		int nMode = 0;
		D3DDISPLAYMODE d3ddm;
		bool bDesiredAdapterModeFound = false;

		// Find the number of modes which suppoert X8R8G8B8
		backbufferFormat = D3DFMT_X8R8G8B8;
		int nMaxAdapterModes = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 );

		for( nMode = 0; nMode < nMaxAdapterModes; ++nMode )
		{
			if( FAILED( m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, nMode, &d3ddm ) ) )
			{
				Cleanup();
				MessageBox(NULL,"Failure enumerating modes","D3D Window",MB_OK);
				return false;
			}

			// Does this adapter mode support a mode of our specified widthxheight?
			if( d3ddm.Width != m_windowWidth || d3ddm.Height != m_windowHeight )
				continue;

			// Does this adapter mode support a 32-bit RGB pixel format?
			if( d3ddm.Format != D3DFMT_X8R8G8B8 )
				continue;

			// Does this adapter mode support a refresh rate of 60 MHz?
			if( d3ddm.RefreshRate < 60 )
				continue;

			// We found a match!
			bDesiredAdapterModeFound = true;
			break;
		}

		if( bDesiredAdapterModeFound == false )
		{
			Cleanup();
			MessageBox(NULL,"Couldn't find desired adapter mode","D3D Window",MB_OK);
			return false;
		}

		//
		// Here's the manual way of verifying hardware support.
		//

		// Can we get a 32-bit back buffer?
		hr = m_pD3D->CheckDeviceType( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_X8R8G8B8,D3DFMT_X8R8G8B8,FALSE );
		if( FAILED( hr ) )
		{
			Cleanup();
			MessageBox(NULL,"Couldn't find mode with specified backbuffer format","D3D Window",MB_OK);
			return false;
		}

		// Can we get a z-buffer that's at least 16 bits?
		hr = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_X8R8G8B8,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,D3DFMT_D16 );
		if( FAILED( hr ))
		{
			Cleanup();
			MessageBox(NULL,"Can't find mode with specified depth format","D3D Window",MB_OK);
			return false;
		}

		//
		// Do we support hardware vertex processing? if so, use it. 
		// If not, downgrade to software.
		//

		D3DCAPS9 d3dCaps;
		hr = m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, &d3dCaps );
		if( FAILED( hr ) )
		{
			Cleanup();
			MessageBox(NULL,"Couldn't get device caps","D3D Window",MB_OK);
			return false;
		}



		//
		// Everything checks out - create a simple, full-screen device.
		//
	}

	hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&m_displayMode);
	hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_displayMode.Format,D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,D3DFMT_D24X8);
	hr = m_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_displayMode.Format,m_displayMode.Format,D3DFMT_D24X8);

	D3DPRESENT_PARAMETERS d3dpp = {0};
	d3dpp.BackBufferWidth = 0;									// Use window width
	d3dpp.BackBufferHeight = 0;									// Use window height
    d3dpp.BackBufferFormat = backbufferFormat;					// Use current device format
	d3dpp.BackBufferCount = 1;									// Single back buffer
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;				// No multisampling
	d3dpp.MultiSampleQuality = 0;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		
	d3dpp.hDeviceWindow = m_hWnd;
    d3dpp.Windowed = !m_fullScreen;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;					// 16 bit Z-biffer
	d3dpp.Flags = 0;											// No extra presentation flags
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// Default interval
    //d3dpp.Windowed = TRUE;
    //d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    //d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    // Create the D3DDevice
	hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice);
	if(FAILED(hr))
	{
		Cleanup();
		MessageBox(NULL,"Could not initialize D3D device","D3D Window",MB_OK);
		return false;
	}

	RenderMgr::GetInstance().SetDXDevice(m_pD3DDevice);
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

	// Destroy D3D handle
	if(m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
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

