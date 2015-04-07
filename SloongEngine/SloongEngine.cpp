// SloongEngine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongEngine.h"


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
