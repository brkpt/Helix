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
	//{
	//	Helix::Array<Foo>	myArray;
	//	Foo bar;
	//	myArray.Add(bar);
	//	myArray.RemoveAt(0);
	//	myArray.SetCount(10);
	//	for(int i=0;i<10;i++)
	//	{
	//		myArray[i] = bar;
	//	}
	//	myArray.RemoveAll();
	//}
	//{
	//	Foo * list[10];

	//	for(int i=0;i<10;i++)
	//		list[i] = new Foo;

	//	for(int i=0;i<10;i++)
	//		delete list[i];

	//}
	//{
	//	Helix::String Foo;
	//	Foo = "ABCD";

	//	Helix::String Bar(Foo);
	//	Bar = Foo + "EFGH";

	//	Foo = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	//}

	TheGame	*game = TheGame::CreateInstance();

	game->Initialize(hInstance,hPrevInstance,lpCmdLine,nCmdShow,false,1024,768,"Matt's D3D App");
	game->LoadScene();
	game->Run();
	game->UnloadScene();
	game->Cleanup();
}
