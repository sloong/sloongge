// SloongEngine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SloongEngine.h"
#include "IUniversal.h"
#include "SloongThreadPool.h"
#include "ISloongObject.h"
#include "SloongCamera.h"

using namespace SoaringLoong;
using namespace SoaringLoong::Universal;
using namespace SoaringLoong::Graphics3D;

#pragma comment(lib,"SloongGraphic.lib")
#pragma comment(lib,"SloongMath.lib")
#pragma comment(lib,"Universal.lib")
typedef struct EVENT_PARAM
{
	int id;
	UI_EVENT event;
}*LPEVENT_PARAM;

typedef struct RENDER_PARAM
{
	IObject*	pObj;
	int			nIndex;
	CCamera*	pCamera;
}*LPRENDER_PARAM;

CThreadPool* g_pThreadPool = nullptr;
CSloongEngine* SoaringLoong::CSloongEngine::theEngine = nullptr;
HANDLE		g_hRenderMutex = INVALID_HANDLE_VALUE;
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
		g_pThreadPool->Initialize(4);
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

DWORD WINAPI SoaringLoong::CSloongEngine::RenderCallBack(LPVOID lpData)
{
	RENDER_PARAM* pTemp = (RENDER_PARAM*)lpData;
	auto pObject = pTemp->pObj;
	auto pCamera = pTemp->pCamera;
	pObject->SetCurrentIndex(pTemp->nIndex);
	pObject->Reset();
	pObject->Cull(pCamera, CULL_MODE::CULL_ON_XYZ_PLANES);
	if (pObject->Visible())
	{
		pObject->ToWorld(TRANS_MODE::LocalToTrans);
		pObject->ToCamera(pCamera);
		pObject->ToProject(pCamera);
		pObject->PerspectiveToScreen(pCamera);
		WaitForSingleObject(g_hRenderMutex, INFINITE);
		pObject->Render();
		ReleaseMutex(g_hRenderMutex);
	}
	SAFE_DELETE(pTemp);
	return 0;
}

void SoaringLoong::CSloongEngine::AddRenderTask(IObject* pObj, int nIndex, CCamera* pCamera)
{
	if ( g_hRenderMutex == INVALID_HANDLE_VALUE )
	{
		g_hRenderMutex = CreateMutex(NULL, FALSE, _T("SloongEngineRenderMutex"));
	}
	RENDER_PARAM* pTemp = new RENDER_PARAM();
	pTemp->pObj = pObj;
	pTemp->nIndex = nIndex;
	pTemp->pCamera = pCamera;
	g_pThreadPool->AddTask(RenderCallBack, pTemp);
}
