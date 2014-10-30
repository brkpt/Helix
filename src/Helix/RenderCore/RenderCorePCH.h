// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <string>
#include <map>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10.h>		// For math structures/functions
#include "LuaPlus.h"

// TODO: reference additional headers your program requires here
#include "Shaders.h"
#include "Materials.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "Textures.h"
#include "InstanceManager.h"
#include "Instance.h"
#include "VDecls.h"
