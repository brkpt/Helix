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
	const float data1[3] = { 3.0f, 3.0f, 3.0f };
	const float data2[3] = { 1.0f, 1.0f, 1.0f };

	Helix::Vector3 v1(data1);
	Helix::Vector3 v2(data2);
	Helix::Vector3 v3;
	Helix::Vector3 v4 = Helix::Vector3(1.0f, 2.0f, 3.0f);

	v3 = v1 + v2;
	v3 = v1 - v2;
	v3 = v1*3.0f;
	v3.Zero();
	v3 = 3.0f*v1;

	v1 = Helix::Vector3(1.0f, 0.0f, 0.0f);
	v2 = Helix::Vector3(0.0f, 1.0f, 0.0f);
	v3 = v1.Cross(v2);

	float dot = v1.Dot(v2);
	dot = Helix::Vector3::Dot(v1, v2);
	v2 = Helix::Vector3(.707f, .707f, 0.f);
	v3 = v1.Cross(v2);

	Helix::Matrix3x3 m1;
	m1.SetTransformZRotation(4.0f * (Helix::PI / 2.f));
	v1 = Helix::Vector3(1.0f, 1.0f, 0.0f);
	v2 = v1*m1;

	OutputDebugString("done");
	

//	TheGame	*game = TheGame::CreateInstance();

//	std::string levelName = lpCmdLine;
//	game->Initialize(hInstance,hPrevInstance,lpCmdLine,nCmdShow,false,1024,768,"Matt's D3D App");
//	game->LoadScene(levelName);
//	game->Run();
//	game->UnloadScene();
//	game->Cleanup();
}
