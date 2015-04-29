// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dxguid.lib")

// TODO: reference additional headers your program requires here
#ifdef SLOONGGRAPHIC_EXPORTS
#define SLOONGGRAPHIC_API __declspec(dllexport)
#else
#define SLOONGGRAPHIC_API __declspec(dllimport)
#endif
