// SloongGraphic.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongGraphic.h"


// This is an example of an exported variable
SLOONGGRAPHIC_API int nSloongGraphic=0;

// This is an example of an exported function.
SLOONGGRAPHIC_API int fnSloongGraphic(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SloongGraphic.h for the class definition
CSloongGraphic::CSloongGraphic()
{
	return;
}
