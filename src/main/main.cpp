// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "TheGame.h"
#include "Utility/bits.h"

#include "Utility/hstring.h"

// ****************************************************************************
// ****************************************************************************
int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	{
		Helix::StringA Foo;
		Foo = "ABCD";

		Helix::StringA Bar(Foo);
		Bar = Foo + "EFGH";

		Foo = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	}

	TheGame	*game = TheGame::CreateInstance();

	game->Initialize(hInstance,hPrevInstance,lpCmdLine,nCmdShow,false,1024,768,"Matt's D3D App");
	game->LoadScene();
	game->Run();
	game->UnloadScene();
	game->Cleanup();
}
