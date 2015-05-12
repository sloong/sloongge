// SloongEngine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongEngine.h"
#include "IUniversal.h"
#include "SloongThreadPool.h"

using namespace SoaringLoong;
using namespace SoaringLoong::Universal;

#pragma comment(lib,"SloongGraphic.lib")
#pragma comment(lib,"SloongMath.lib")
#pragma comment(lib,"Universal.lib")
typedef struct EVENT_PARAM
{
	int id;
	UI_EVENT event;
}*LPEVENT_PARAM;

CThreadPool* g_pThreadPool = nullptr;
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
	if ( !g_pThreadPool )
	{
		g_pThreadPool = new CThreadPool();
		g_pThreadPool->Initialize(15);
		g_pThreadPool->Start();

	}
	if ( theEngine && theEngine->g_EventFunc )
	{
		EVENT_PARAM* temp = new EVENT_PARAM();
		temp->id = id;
		temp->event = args;
		g_pThreadPool->AddTask(theEngine->g_EventFunc,temp);
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

int SoaringLoong::CSloongEngine::GetEventListTotal()
{
	return g_pThreadPool->GetTaskTotal();
}
