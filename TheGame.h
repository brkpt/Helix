#ifndef THEGAME_DOT_H
#define THEGAME_DOT_H

#include "Win32DX9.h"

#define	KEY_COUNT		256

class Triangle;
class Grid;
class Camera;

struct MouseState {
	short	mouseDeltaX;
	short	mouseDeltaY;
	short	mousePosX;
	short	mousePosY;
	union  {
		DWORD	flags;
		struct {
			bool	leftButtonDown : 1;
			bool	rightButtonDown : 1;
			bool	middleButtonDown : 1;
		};
	};
};

class TheGame : public WinApp
{
public:
	TheGame(void);

	static TheGame *	CreateInstance(void);
	static TheGame *	Instance(void)			{ return m_instance; }

	bool		Initialize(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow,bool fullScreen,int width,int height,LPTSTR windowName);
	void		Update(void);
	void		Render(void);
	void		LoadScene(void);
	void		UnloadScene(void);
	Camera *	CurrentCamera(void) { return m_camera; }

	void				ProcessMouseMove(LPARAM lParam, WPARAM wParam);
	void				ProcessMouseLBDown(LPARAM lParam, WPARAM wParam);
	void				ProcessMouseLBUp(LPARAM lParam, WPARAM wParam);
	void				ProcessMouseRBDown(LPARAM lParam, WPARAM wParam);
	void				ProcessMouseRBUp(LPARAM lParam, WPARAM wParam);
	void				ProcessMouseMBDown(LPARAM lParam, WPARAM wParam);
	void				ProcessMouseMBUp(LPARAM lParam, WPARAM wParam);
	void				ProcessKeyDown(LPARAM lParam, WPARAM wParam);
	void				ProcessKeyUp(LPARAM lParam, WPARAM wParam);
	bool				KeyDown(unsigned short key) { return m_keyboardState[key]; }
	const MouseState &	MouseState(void)	{ return m_mouseState; }

protected:

	static TheGame *	m_instance;
	Triangle *			m_triangle;
	Grid *				m_grid;
	Camera *			m_camera;
	bool				m_keyboardState[KEY_COUNT];
	struct MouseState	m_mouseState;
};

#endif // THEGAME_DOT_H