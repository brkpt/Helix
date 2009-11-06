// ****************************************************************************
// ****************************************************************************
#include "stdafx.h"
#include "TheGame.h"
#include "Utility/bits.h"

#include "Utility/String/String.h"
#include "Utility/Memory/FixedAlloc.h"
#include "Utility/Container/Array.h"

class Foo
{
public:
	Foo()
	{
		index = ++totalCount;
	}

	Foo(const Foo &other)
	{
		index = ++totalCount;
	}

	~Foo()
	{
		index = 0;
	}

	const Foo &operator=(const Foo &other)
	{
		index = ++totalCount;
		return *this;
	}
	DECLARE_FIXED_ALLOC(Foo);

private:
	char	fooData[100];
	int		index;

	static int	totalCount;
};

IMPLEMENT_FIXED_ALLOC(Foo,5);

int Foo::totalCount = 0;


// ****************************************************************************
// ****************************************************************************
int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	TheGame	*game = TheGame::CreateInstance();

	std::string levelName = lpCmdLine;
	game->Initialize(hInstance,hPrevInstance,lpCmdLine,nCmdShow,false,1024,768,"Matt's D3D App");
	game->LoadScene(levelName);
	game->Run();
	game->UnloadScene();
	game->Cleanup();
}
