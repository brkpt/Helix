// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX_DOT_H


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define DIRECT3D_VERSION         0x0900

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <crtdbg.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <d3d11.h>
#include <d3dx10.h>
#include "LuaPlus.h"
#include "Kernel/Win32DX10.h"
#include "RenderCore/RenderThread.h"
#include "RenderCore/VDecls.h"
#include "RenderCore/InstanceManager.h"
#include "RenderCore/Shaders.h"
#include "RenderCore/Textures.h"
#include "RenderCore/Materials.h"
#include "RenderCore/MeshManager.h"
#include "RenderCore/Instance.h"
#include "RenderCore/Mesh.h"

// TODO: reference additional headers your program requires here

#endif
