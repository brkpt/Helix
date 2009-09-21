#ifndef WIN32DX9_DOT_H
#define WIN32DX9_DOT_H

#include "resource.h"

#define MAX_LOADSTRING	100

class WinApp 
{
public:
	WinApp(void);
	~WinApp(void);

	static WinApp *		Instance(void) { return m_instance; }
	void				Cleanup(void);
	HINSTANCE			AppInstance(void)			{ return m_appInstance; }
	ID3D10Device *		GetDevice(void)	{ return m_pD3DDevice; }
	unsigned short		WindowWidth(void)	{ return (unsigned short)m_windowWidth; }
	unsigned short		WindowHeight(void)	{ return (unsigned short)m_windowHeight; }

	static LRESULT CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual bool		Initialize(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow,bool fullScreen,int width,int height,LPTSTR windowName);
	virtual int				Run(void);
	virtual void			Update(void);
	virtual void			Render(void);

	virtual void	ProcessMouseMove(LPARAM lParam, WPARAM wParam) = 0;
	virtual void	ProcessKeyDown(LPARAM lparam, WPARAM wParam) = 0;
	virtual void	ProcessKeyUp(LPARAM lparam, WPARAM wParam) = 0;
	virtual void	ProcessMouseLBDown(LPARAM lParam, WPARAM wParam) = 0;
	virtual void	ProcessMouseLBUp(LPARAM lParam, WPARAM wParam) = 0;
	virtual void	ProcessMouseRBDown(LPARAM lParam, WPARAM wParam) = 0;
	virtual void	ProcessMouseRBUp(LPARAM lParam, WPARAM wParam) = 0;
	virtual void	ProcessMouseMBDown(LPARAM lParam, WPARAM wParam) = 0;
	virtual void	ProcessMouseMBUp(LPARAM lParam, WPARAM wParam) = 0;


protected:
	bool			InitializeWin32(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow, bool fullScreen, unsigned short width, unsigned short height, LPTSTR windowTitle);
	void			CleanupWin32(void);
	bool			InitializeDirectX(void);
	void			CleanupDirectX(void);

	static WinApp *	m_instance;
	HINSTANCE		m_appInstance;
	TCHAR			m_szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR			m_szWindowClass[MAX_LOADSTRING];			// the main window class name
	HWND			m_hWnd;
	HACCEL			m_hAccelTable;
	bool			m_fullScreen;
	int				m_windowWidth, m_windowHeight;
	HDC				m_hDC;

	ID3D10Device *	m_pD3DDevice;
};

#endif
