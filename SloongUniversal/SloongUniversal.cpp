// SloongUniversal.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongUniversal.h"


// This is an example of an exported variable
SLOONGUNIVERSAL_API int nSloongUniversal=0;

// This is an example of an exported function.
SLOONGUNIVERSAL_API int fnSloongUniversal(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SloongUniversal.h for the class definition
CSloongUniversal::CSloongUniversal()
{
	return;
}
