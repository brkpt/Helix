// ****************************************************************************
// ****************************************************************************
#include "TheGame.h"
#include "Utility/bits.h"
#include "Utility/String/String.h"
#include "Utility/Memory/FixedAlloc.h"
#include "Utility/Container/Array.h"
#include "Math/MathDefs.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

// ****************************************************************************
// ****************************************************************************
int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	Helix::Matrix4x4 m1;
	m1.SetZRotation(Helix::PI / 4.0f);
	float det = m1.Determinant();

	Helix::Matrix4x4 m2 = m1;
	bool inverted = m2.Invert();

	Helix::Matrix4x4 m3;
	Helix::Vector4 v1(10.0f, 5.0f, 3.0f);
	m3.SetTranslation(v1);
	Helix::Matrix4x4 m4 = m3;
	inverted = m4.Invert();

	TheGame	*game = TheGame::CreateInstance();

	std::string levelName = lpCmdLine;
	game->Initialize(hInstance,hPrevInstance,lpCmdLine,nCmdShow,false,1024,768,"Matt's D3D App");
	game->LoadScene(levelName);
	game->Run();
	game->UnloadScene();
	game->Cleanup();
}

