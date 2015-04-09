// SloongEngine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongEngine.h"
#include "IUniversal.h"

using namespace SoaringLoong;

// This is an example of an exported variable
SLOONGENGINE_API int nSloongEngine=0;

// This is an example of an exported function.
SLOONGENGINE_API int fnSloongEngine(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SloongEngine.h for the class definition
CSloongEngine::CSloongEngine()
{
	return;
}

bool CSloongEngine::InRect(const CSize& pos, const CRect& rc)
{
	if ((pos.cx < rc.left) || (pos.cx > rc.right))
		return false;
	if ((pos.cy < rc.top) || (pos.cy > rc.bottom))
		return false;

	return true;
}