// SloongEngine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongEngine.h"
#include "IUniversal.h"

using namespace SoaringLoong;

#pragma comment(lib,"SloongGraphic.lib")
#pragma comment(lib,"SloongMath.lib")
#pragma comment(lib,"Universal.lib")

CSloongEngine* SoaringLoong::CSloongEngine::theEngine = nullptr;
// This is the constructor of a class that has been exported.
// see SloongEngine.h for the class definition
CSloongEngine::CSloongEngine()
{
	theEngine = this;
}

bool CSloongEngine::InRect(const CSize& pos, const CRect& rc)
{
	if ((pos.cx < rc.left) || (pos.cx > rc.right))
		return false;
	if ((pos.cy < rc.top) || (pos.cy > rc.bottom))
		return false;

	return true;
}

void SoaringLoong::CSloongEngine::SendEvent(int id, UI_EVENT args)
{
	if ( theEngine && theEngine->g_EventFunc )
	{
		theEngine->g_EventFunc(id, args);
	}
}

void SoaringLoong::CSloongEngine::SetEnentHandler(EventFunc func)
{
	g_EventFunc = func;
}

SoaringLoong::CSloongEngine::~CSloongEngine()
{
	theEngine = nullptr;
}
