// SloongMath.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongMath.h"


// This is an example of an exported variable
SLOONGMATH_API int nSloongMath=0;

// This is an example of an exported function.
SLOONGMATH_API int fnSloongMath(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SloongMath.h for the class definition
CSloongMath::CSloongMath()
{
	return;
}
